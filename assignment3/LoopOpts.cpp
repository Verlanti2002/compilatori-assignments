#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Dominators.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/Twine.h"

using namespace llvm;

const bool TEST = true;

namespace {


// New PM implementation
struct LoopOpts: PassInfoMixin<LoopOpts> {
  // Main entry point, takes IR unit to run the pass on (&F) and the
  // corresponding pass manager (to be queried if need be)
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {

    // Get LoopInfo object
    LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
    // Get DominatorTree object
    DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);

    return moveLoopInvariantInstructions(LI, DT);
  }

  /* Check if an instruction I is Loop Invariant with respect to loop L */
  bool isLoopInvariant(Instruction &I, Loop &L, std::set<Instruction *> &MI) {
    for (auto &Op : I.operands()) {
      if(isa<Constant>(Op)) continue;
      
      if (Instruction *InstOp = dyn_cast<Instruction>(Op)) {
        if (L.contains(InstOp) && !MI.count(InstOp)){
          printLogs("Loop Variant Instruction!");
          return false;
        }
      }
    }

    printLogs("Loop Invariant Instruction!");
    return true;
  }

  /* Check if an instruction I Dominates All Exits of the loop L */
  bool dominatesAllLoopExits(DominatorTree &DT, Loop &L, Instruction &I){
    SmallVector<BasicBlock *> ExitBlocks;
    L.getExitBlocks(ExitBlocks);

    for(BasicBlock *ExitBB : ExitBlocks){
      if(!DT.dominates(I.getParent(), ExitBB)){
        printLogs("Does NOT Dominate All Loop Exits!");
        return false;
      }
    }

    printLogs("Dominate All Loop Exits!");
    return true;
  }

  /* Check that the instruction being moved out of the loop is not used inside the loop by another instruction that would be executed before */
  bool isSafeToMove(Loop &L, DominatorTree &DT, Instruction &I){
    for(User *U : I.users()){
      if(Instruction *UserI = dyn_cast<Instruction>(U)){
        if(L.contains(UserI->getParent()) && !DT.dominates(I.getParent(), UserI->getParent())){ // Check if the user is in the loop and does not dominate the instruction (UserI executed before I) it's not safe to move
          printLogs("NOT Safe To Move!");
          return false; 
        }
      }
    }
    
    printLogs("Safe To Move!");
    return true; 
  }

  /* Check if an instruction I is Dead Outside the Loop L (has no uses outside the loop L) */
  bool isDeadOutsideLoop(Loop &L, Instruction &I){
    for(User *U : I.users()){
      if(Instruction *UserI = dyn_cast<Instruction>(U)){
        if(!L.contains(UserI->getParent())){ // Check if the user's parent block is outside the loop
          printLogs("NOT Dead Outside Loop!");
          return false; // Found a use outside the loop: not dead outside
        }
      }
    }
    printLogs("Dead Outside Loop Instruction!");
    return true; // All uses are inside the loop: dead outside
  }

  void printInstruction(const llvm::Twine &msg, const Instruction *I){
    if (TEST) {
      std::string instrStr;
      llvm::raw_string_ostream rso(instrStr);
      I->print(rso);
      rso.flush();

      outs() << msg << instrStr << "\n";
    }
  }

  void printLogs(const llvm::Twine &msg) {
    if (TEST)
      outs() << msg << "\n";
  }

  /* Main function to move loop-invariant instructions to the preheader of the loop */ 
  PreservedAnalyses moveLoopInvariantInstructions(LoopInfo &LI, DominatorTree &DT) {
    bool Changed = false;
    std::set<Instruction *> MovedInstructions; // Set to track instructions that have already been moved

    for (Loop *L : LI.getLoopsInPreorder()) {
      if (!L->isLoopSimplifyForm() || !L->getLoopPreheader()) continue;

      BasicBlock *Preheader = L->getLoopPreheader();
      bool LocalChange; // Flag to indicate if there was a change in this iteration

      do {
        LocalChange = false; 
        SmallVector<Instruction *> ToMove; // List of instructions to move

        for (BasicBlock *BB : L->blocks()) {
          for (Instruction &I : *BB) {
            printInstruction("Instruction: ", &I);
            if (I.isTerminator() || isa<PHINode>(&I) || I.mayReadOrWriteMemory() || I.mayHaveSideEffects()) continue;

            if (!isLoopInvariant(I, *L, MovedInstructions)) continue;

            if (!dominatesAllLoopExits(DT, *L, I) && !isDeadOutsideLoop(*L, I)) continue;

            if (!isSafeToMove(*L, DT, I)) continue;

            printLogs("LICM enabled: Added instruction to ToMove vector!");
            ToMove.push_back(&I);
          }
        }

        for (Instruction *I : ToMove) {
          printInstruction("INSTRUCTION MOVED: ", I);
          I->moveBefore(Preheader->getTerminator()); // Move the instruction before the preheader terminator
          MovedInstructions.insert(I);
          Changed = LocalChange = true; // Set the flags to indicate a change
        }
      } while (LocalChange); // Continue as long as there are changes being made
    }

    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
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
  return {LLVM_PLUGIN_API_VERSION, "LoopOpts", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "loop-opts") {
                    FPM.addPass(LoopOpts());
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
