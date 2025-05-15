#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/Analysis/DependenceAnalysis.h"

using namespace llvm;

namespace {


// New PM implementation
struct LoopFusionOpts: PassInfoMixin<LoopFusionOpts> {
  // Main entry point, takes IR unit to run the pass on (&F) and the
  // corresponding pass manager (to be queried if need be)
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    bool Changed = false;

    // Get LoopInfo object
    LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
    // Get DominatorTree object
    DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
    // Get PostDominatorTree object
    PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);
    // Get ScalarEvolution object
    ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);
    // Get DependenceInfo object
    DependenceInfo &DI = AM.getResult<DependenceAnalysis>(F);

    SmallVector<Loop *, 8> AllLoops;

    // Recupera tutti i loop
    for (Loop *L : LI) { 
      for (Loop *SubLoop : depth_first(L)) { // Visita la gerarchia dei loop
        AllLoops.insert(AllLoops.begin(), SubLoop);
      }
    }

    // Esamina coppie consecutive per vedere se sono adiacenti
    for (size_t i = 0; i < AllLoops.size() - 1; ++i) {
      Loop *L1 = AllLoops[i];
      Loop *L2 = AllLoops[i + 1];
  
      if (!areLoopsAdjacent(L1, L2)) {
        errs() << "Loop " << (i+1) << " and Loop " << (i+2) << " are NOT adjacent\n";
        continue;
      }
  
      if (!areControlFlowEquivalent(L1, L2, DT, PDT)) {
        errs() << "Loop " << (i+1) << " and Loop " << (i+2) << " are NOT control flow equivalent\n";
        continue;
      }
  
      if (!haveSameTripCount(L1, L2, SE)) {
        errs() << "Trip counts are different\n";
        continue;
      }
  
      if (hasNegativeDistanceDependence(L1, L2, DI, SE)) {
        errs() << "Negative distance dependence detected\n";
        continue;
      }
  
      // Se arriviamo qui, i loop sono fusibili
      errs() << "Loop " << (i+1) << " and Loop " << (i+2) << " are FUSIBLE. Proceeding with fusion...\n";
      Changed = fuseLoops(L1, L2, LI);
    }

    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }

  BasicBlock* getLoopEntryBlock(Loop *L) {
    if (L->isGuarded())
      return L->getLoopGuardBranch()->getParent();
    else
      return L->getLoopPreheader();
  }

  bool areLoopsAdjacent(Loop *L1, Loop *L2) {
    BasicBlock *L1ExitBlock = L1->getExitBlock();
    BasicBlock *L2EntryBlock = getLoopEntryBlock(L2);

    // Caso NON Guarded
    if (L1ExitBlock && L2EntryBlock && L1ExitBlock == L2EntryBlock) {
      return true; // L2 parte esattamente dove finisce L1, non c'è guardia
    }

    // Caso Guarded (l'esecuzione del corpo del loop dipende da una condizione)
    if (L1->isGuarded()) {
      BranchInst *GuardBranch = L1->getLoopGuardBranch();
    
      for (unsigned i = 0; i<GuardBranch->getNumSuccessors(); ++i) {
        if (GuardBranch->getSuccessor(i) == L2EntryBlock)
          return true;
      }
    }

    return false;
  }

  bool areControlFlowEquivalent(Loop *L1, Loop *L2, DominatorTree &DT, PostDominatorTree &PDT) {
    // Verifica se L1 domina L2 e L2 postdomina L1
    if (DT.dominates(L1->getHeader(), L2->getHeader()) && PDT.dominates(L2->getHeader(), L1->getHeader()))
      return true;

    return false;
  }

  bool haveSameTripCount(Loop *L1, Loop *L2, ScalarEvolution &SE) {
    const SCEV *TripCountL1 = SE.getBackedgeTakenCount(L1);
    const SCEV *TripCountL2 = SE.getBackedgeTakenCount(L2);

    if (TripCountL1 == TripCountL2)
      return true;

    return false;
  }

  /* Condizione vietata per la loop fusion, significherebbe che L1 ha bisogno di un valore che L0 produrrà in una iterazione futura */
  bool hasNegativeDistanceDependence(Loop *L1, Loop *L2, DependenceInfo &DI, ScalarEvolution &SE) {
    for (BasicBlock *BB1 : L1->blocks()) {
      for (Instruction &I1 : *BB1) {
        for (BasicBlock *BB2 : L2->blocks()) {
          for (Instruction &I2 : *BB2) {
            std::unique_ptr<Dependence> Dep = DI.depends(&I1, &I2, true);
            if (Dep && Dep->isOrdered()) {
              for (unsigned Level = 0; Level < Dep->getLevels(); ++Level) {
                const SCEV *DistSCEV = Dep->getDistance(Level);
                if (!DistSCEV)
                  continue;
  
                if (const SCEVConstant *Dist = dyn_cast<SCEVConstant>(DistSCEV)) {
                  if (Dist->getAPInt().isNegative()) {
                    errs() << "Negative dependence from: " << I1 << " to " << I2 << " distance: " << *Dist << "\n";
                    return true;
                  }
                }
              }
            }
          }
        }
      }
    }
  
    return false;
  }

  bool fuseLoops(Loop *L1, Loop *L2, LoopInfo &LI) {
    // Recupera le variabili di induzione (contatori) da entrambi i loop
    PHINode *IndVarL1 = L1->getCanonicalInductionVariable();
    PHINode *IndVarL2 = L2->getCanonicalInductionVariable();

    if (!IndVarL1 || !IndVarL2) {
      errs() << "One of the loops does not have a canonical induction variable. Fusion not safe.\n";
      return false;
    }

    // Sostituisce gli usi della IV di l2 SOLO nel corpo di l2
    for (User *U : IndVarL2->users()) {
      if (Instruction *Inst = dyn_cast<Instruction>(U)) {
        BasicBlock *Parent = Inst->getParent();
        if (L2->contains(Parent))
          Inst->replaceUsesOfWith(IndVarL2, IndVarL1);
      }
    }

    // Sostituisce tutti gli usi della IV di L2 con quella di L1 se tutti sono interni a L2
    bool canReplace = true;
    for (User *U : IndVarL2->users()) {
      if (auto *I = dyn_cast<Instruction>(U)) {
        if (!L2->contains(I)) {
          canReplace = false;
          break;
        }
      } else {
        canReplace = false;
        break;
      }
    }

    if (canReplace) {
      IndVarL2->replaceAllUsesWith(IndVarL1);
      IndVarL2->eraseFromParent();
    }

    // Modifica il CFG per collegare il body di L2 dopo quello di L1
    BasicBlock *L1Latch = L1->getLoopLatch();
    BasicBlock *L2Header = L2->getHeader();
    BasicBlock *L2Preheader = L2->getLoopPreheader();
    BasicBlock *L2Latch = L2->getLoopLatch();
    BasicBlock *L2Exit = L2->getExitBlock();

    if (!L1Latch || !L2Header || !L2Preheader || !L2Latch || !L2Exit) {
      errs() << "Missing one or more critical blocks (header, latch, exit) from L1 or L2. Cannot perform fusion.\n";
      return false;
    }

    // Rimuove il salto dal latch di L1 che torna all'header e lo fa puntare all'header di L2
    for (BasicBlock *Pred : predecessors(L1Latch)) {
      if (L1->contains(Pred))
        Pred->getTerminator()->replaceSuccessorWith(L1Latch, L2Header);
    }

    // Ricollega i blocchi all'interno di L2 che puntano al latch L2 affinché puntino al latch di L1
    for (BasicBlock *Pred : predecessors(L2Latch)) {
      if (L2->contains(Pred)) 
        Pred->getTerminator()->replaceSuccessorWith(L2Latch, L1Latch);
    }

    // Trasferisce i blocchi interni di L2 (esclusi header e latch) dentro L1
    SmallVector<BasicBlock *, 8> BlocksToMove;
    for (BasicBlock *BB : L2->blocks()) {
      if (BB != L2Header && BB != L2Latch)
        BlocksToMove.push_back(BB);
    }

    for (BasicBlock *BB : BlocksToMove) {
      L2->removeBlockFromLoop(BB);
      L1->addBasicBlockToLoop(BB, LI);
    }

    return true;
  }

  // Without isRequired returning true, this pass will be skipped for functions
  // decorated with the optnone LLVM attribute. Note that clang -O0 decorates
  // all functions with optnone.
  static bool isRequired() { return true; }
};
} // namespace

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getTestPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LoopFusionOpts", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "loop-fusion-opts") {
                    FPM.addPass(LoopFusionOpts());
                    return true;
                  }
                  return false;
                });
          }};
}

// This is the core interface for pass plugins. It guarantees that 'opt' will
// be able to recognize TestPass when added to the pass pipeline on the
// command line, i.e. via '-passes=test-pass'
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getTestPassPluginInfo();
}
