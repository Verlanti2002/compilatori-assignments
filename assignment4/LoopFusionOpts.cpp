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
#include "llvm/ADT/Twine.h"

using namespace llvm;

const bool TEST = true;

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

    // Get all loops
    for (Loop *L : LI) { 
      for (Loop *SubLoop : depth_first(L)) { // Visit the loop hierarchy
        AllLoops.insert(AllLoops.begin(), SubLoop);
      }
    }

    // Examine consecutive pairs to see if they are adjacent
    for (size_t i = 0; i < AllLoops.size() - 1; ++i) {
      Loop *L1 = AllLoops[i];
      Loop *L2 = AllLoops[i + 1];

      if (!areControlFlowEquivalent(L1, L2, DT, PDT)) continue;
  
      if (!areLoopsAdjacent(L1, L2)) continue;
  
      if (!haveSameTripCount(L1, L2, SE)) continue;
  
      if (hasNegativeDistanceDependence(L1, L2, DI, SE)) continue;
  
      // Loops are fusible
      errs() << "Loop " << (i+1) << " and Loop " << (i+2) << " are FUSIBLE. Proceeding with fusion...\n";
      Changed = fuseLoops(L1, L2, LI);
    }

    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }

  bool areLoopsAdjacent(Loop *L1, Loop *L2) {
    if ((L1->getExitBlock() == L2->getLoopPreheader()) && (L1->getExitBlock()->size() == 1)){
      printLogs("Loop 1 and Loop 2 are Adjacent!");
      return true;
    }

    printLogs("Loop 1 and Loop 2 are NOT Adjacent!");
    return false;
  }

  bool areControlFlowEquivalent(Loop *L1, Loop *L2, DominatorTree &DT, PostDominatorTree &PDT) {
    // Check if L1 dominates L2 and L2 postdominates L1
    if (DT.dominates(L1->getHeader(), L2->getHeader()) && PDT.dominates(L2->getHeader(), L1->getHeader())){
      printLogs("Loop 1 and Loop 2 are Control Flow Equivalent!");
      return true;
    }

    printLogs("Loop 1 and Loop 2 are NOT Control Flow Equivalent!");
    return false;
  }

  bool haveSameTripCount(Loop *L1, Loop *L2, ScalarEvolution &SE) {
    const SCEV *TripCountL1 = SE.getBackedgeTakenCount(L1);
    const SCEV *TripCountL2 = SE.getBackedgeTakenCount(L2);

    if (TripCountL1 == TripCountL2){
      printLogs("Loop 1 and Loop 2 have Same Trip Count!");
      return true;
    }

    printLogs("Loop 1 and Loop 2 NOT have Same Trip Count!");
    return false;
  }

  /* The extractOffsetFromGEP function is intended to extract any constant offset from an LLVM instruction that accesses memory, such as a load or store. */
  int extractOffsetFromGEP(Instruction &Src) {
    Value *Ptr = nullptr;
    if (auto *Load = dyn_cast<LoadInst>(&Src))
      Ptr = Load->getPointerOperand(); // gets the pointer from which it loads
    else if (auto *Store = dyn_cast<StoreInst>(&Src))
      Ptr = Store->getPointerOperand(); // gets the pointer to which it stores
    else
      return 0;

    Ptr = Ptr->stripPointerCasts(); // removes any pointer casts

    auto *GEP = dyn_cast<GetElementPtrInst>(Ptr); // instruction typically used to access array elements
    if (!GEP || GEP->getNumIndices() < 1)
      return 0;

    Value *Index = GEP->getOperand(GEP->getNumOperands() - 1); // extracts the last operand of the GEP

    if (auto *CI = dyn_cast<ConstantInt>(Index))
      return CI->getSExtValue();

    if (auto *Op = dyn_cast<BinaryOperator>(Index)) {
      if (Op->getOpcode() == Instruction::Add || Op->getOpcode() == Instruction::Sub) {
        Value *LHS = Op->getOperand(0);
        Value *RHS = Op->getOperand(1);

        ConstantInt *Const = dyn_cast<ConstantInt>(RHS);
        if (!Const)
            Const = dyn_cast<ConstantInt>(LHS);

        if (Const) {
            int64_t Offset = Const->getSExtValue();
            if (Op->getOpcode() == Instruction::Sub && Const == RHS)
                Offset = -Offset;
            return Offset;
        }
      }
    }

    return 0;
  }

  /* Forbidden condition for loop fusion, it would mean that L1 needs a value that L0 will produce in a future iteration */
  bool hasNegativeDistanceDependence(Loop *L1, Loop *L2, DependenceInfo &DI, ScalarEvolution &SE) {

    for (BasicBlock *BB1 : L1->blocks()) {
      for (BasicBlock *BB2 : L2->blocks()) {
        for (Instruction &I1 : *BB1) {
          // Consider only memory accesses (load and store are array accesses)
          if (!isa<LoadInst>(&I1) && !isa<StoreInst>(&I1))
            continue;

          for (Instruction &I2 : *BB2) {
            if (!isa<LoadInst>(&I2) && !isa<StoreInst>(&I2))
              continue;

            if (auto Dep = DI.depends(&I1, &I2, true)) { // checks if there is a data dependency between the two instructions
              if(isDistanceNegative(Dep, I2, I1)){ // checks if it is a negative dependency
                printLogs("Loop 1 and Loop 2 have Negative Distance Dependence!");
                return true;
              }
            }
          }
        }
      }
    }

    printLogs("Loop 1 and Loop 2 have NOT Negative Distance Dependence!");
    return false;
  }

  bool isDistanceNegative(std::unique_ptr<Dependence> &Dep, Instruction &Src, Instruction &Dst) {
    if (extractOffsetFromGEP(Dst) - extractOffsetFromGEP(Src) != 0)
      return true;
    return false;
  }

  bool fuseLoops(Loop *L1, Loop *L2, LoopInfo &LI) {
    // Get the induction variables (counters) from both loops
    PHINode *IndVarL1 = L1->getCanonicalInductionVariable();
    PHINode *IndVarL2 = L2->getCanonicalInductionVariable();

    if (!IndVarL1 || !IndVarL2) {
      printLogs("One of the loops does not have a canonical induction variable. Fusion not safe!");
      return false;
    }

    // Replace uses of the IV of l2 ONLY in the body of l2
    for (User *U : IndVarL2->users()) {
      if (Instruction *Inst = dyn_cast<Instruction>(U)) {
        BasicBlock *Parent = Inst->getParent();
        if (L2->contains(Parent))
          Inst->replaceUsesOfWith(IndVarL2, IndVarL1);
      }
    }

    // Replace all uses of the IV of L2 with that of L1 if all are internal to L2
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

    // Modify the CFG to connect the body of L2 after that of L1
    BasicBlock *L1Latch = L1->getLoopLatch();
    BasicBlock *L2Header = L2->getHeader();
    BasicBlock *L2Preheader = L2->getLoopPreheader();
    BasicBlock *L2Latch = L2->getLoopLatch();
    BasicBlock *L2Exit = L2->getExitBlock();

    if (!L1Latch || !L2Header || !L2Preheader || !L2Latch || !L2Exit) {
      printLogs("Missing one or more critical blocks (header, latch, exit) from L1 or L2. Cannot perform fusion!");
      return false;
    }

    // Remove the jump from L1’s latch that goes back to the header and make it point to the header of L2
    for (BasicBlock *Pred : predecessors(L1Latch)) {
      if (L1->contains(Pred))
        Pred->getTerminator()->replaceSuccessorWith(L1Latch, L2Header);
    }

    // Reconnect the blocks inside L2 that point to the L2 latch so that they point to the L1 latch
    for (BasicBlock *Pred : predecessors(L2Latch)) {
      if (L2->contains(Pred)) 
        Pred->getTerminator()->replaceSuccessorWith(L2Latch, L1Latch);
    }

    // Move the internal blocks of L2 (excluding header and latch) into L1
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

  void printLogs(const llvm::Twine &msg) {
    if (TEST)
      outs() << msg << "\n";
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
