#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"

using namespace llvm;

namespace {


// New PM implementation
struct LocalOptsPass: PassInfoMixin<LocalOptsPass> {
  // Main entry point, takes IR unit to run the pass on (&F) and the
  // corresponding pass manager (to be queried if need be)
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {

  	bool Transformed = runOnFunction(F);
  	return (Transformed ? PreservedAnalyses::none() : PreservedAnalyses::all());
  }

  bool runOnBasicBlock(BasicBlock &B) {
    bool Transformed = false;
    
    /* Itera le istruzioni del BasicBlock */
    for(auto It = B.begin(); It != B.end(); ) {
      Instruction &I = *It++;
      /* Verifica se l'operazione è di tipo binario */
    	if(auto *BO = dyn_cast<BinaryOperator>(&I)){
          /* Verifica se l'operazione binaria è un'addizione o una moltiplicazione */
    		  if(BO->getOpcode() == Instruction::Add || BO->getOpcode() == Instruction::Mul){
    			  Value *LHS = BO->getOperand(0);
    			  Value *RHS = BO->getOperand(1);
          /* Se l'operazione è un'addizione...*/
    			if(BO->getOpcode() == Instruction::Add){
            /* Ottimizzazione x+0 -> x */
    				if(auto *C = dyn_cast<ConstantInt>(RHS)){
              /* Se l'operando RHS di tipo ConstantInt è zero allora si procede al replace */
    					if(C->getValue().isZero()){
    						llvm::errs() << "Ottimizzazione applicata: " << *BO << " -> " << *LHS << "\n";
                BO->replaceAllUsesWith(LHS);
                BO->eraseFromParent();
    						Transformed = true;
    					}
            /* Ottimizzazione 0+x -> x */ 
    				} else if(auto *C = dyn_cast<ConstantInt>(LHS)) {
              /* Se l'operando LHS di tipo ConstantInt è 0 allora si procede al replace */
    					if(C->getValue().isZero()){
                llvm::errs() << "Ottimizzazione applicata: " << *BO << " -> " << *RHS << "\n";
    						BO->replaceAllUsesWith(RHS);
                BO->eraseFromParent();
    						Transformed = true;
    					}
    				}
            /* Se l'operazione è una moltiplicazione...*/
    			} else {  
            /* Ottimizzazione x*1 -> x */
    				if(auto *C = dyn_cast<ConstantInt>(RHS)) {
              /* Se l'operando RHS di tipo ConstantInt è 1 allora si procede al replace */
    				 	if(C->getValue().isOne()){
                llvm::errs() << "Ottimizzazione applicata: " << *BO << " -> " << *LHS << "\n";
    						BO->replaceAllUsesWith(LHS);
                BO->eraseFromParent();
                Transformed = true;
    					}
            /* Ottimizzazione 1*x -> x */
    				} else if(auto *C = dyn_cast<ConstantInt>(LHS)) { 
              /* Se l'operando LHS di tipo ConstantInt è 1 allora si procede al replace */
    					if(C->getValue().isOne()) {
                llvm::errs() << "Ottimizzazione applicata: " << *BO << " -> " << *RHS << "\n";
    						BO->replaceAllUsesWith(RHS);
                BO->eraseFromParent();
    						Transformed = true;
    					}
    				}
    			}
    		}
    	}
    }
    
    return Transformed;
  }

  bool runOnFunction(Function &F) {
    bool Transformed = false;
    for (auto &BB : F) {
      if (runOnBasicBlock(BB)) {
        Transformed = true;
      }
    }
    return Transformed;
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
  return {LLVM_PLUGIN_API_VERSION, "LocalOptsPass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "local-opts") {
                    FPM.addPass(LocalOptsPass());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getTestPassPluginInfo();
}
