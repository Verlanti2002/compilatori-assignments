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
    	if(auto *BO = dyn_cast<BinaryOperator>(&I)) {
        Value *LHS = BO->getOperand(0);
        Value *RHS = BO->getOperand(1);
        /* Verifica se l'operazione binaria è un'addizione o una moltiplicazione */
    		if(BO->getOpcode() == Instruction::Add || BO->getOpcode() == Instruction::Mul) { 
          /* Se l'operazione è un'addizione...*/
    			if(BO->getOpcode() == Instruction::Add) {
            /* -- ALGEBRAIC IDENTITY x+0 -> x -- */
    				if(auto *C = dyn_cast<ConstantInt>(RHS)) { 
              /* Se l'operando RHS di tipo ConstantInt è zero allora si procede al replace */
    					if(C->getValue().isZero()) {
    						llvm::errs() << "Ottimizzazione applicata: " << *BO << " -> " << *LHS << "\n";
                BO->replaceAllUsesWith(LHS);
                BO->eraseFromParent();
    						Transformed = true;
    					}
              /* -- ALGEBRAIC IDENTITY 0+x -> x -- */ 
    				  } else if(auto *C = dyn_cast<ConstantInt>(LHS)) {
                /* Se l'operando LHS di tipo ConstantInt è 0 allora si procede al replace */
    					  if(C->getValue().isZero()) {
                  llvm::errs() << "Ottimizzazione applicata: " << *BO << " -> " << *RHS << "\n";
    						  BO->replaceAllUsesWith(RHS);
                  BO->eraseFromParent();
    						  Transformed = true;
    					  }
    				  }
              /* Se l'operazione è una moltiplicazione...*/
    			  } else {  
              /* -- ALGEBRAIC IDENTITY x*1 -> x -- */
    				  if(auto *C = dyn_cast<ConstantInt>(RHS)) {
                /* Se l'operando RHS di tipo ConstantInt è 1 allora si procede al replace */
    				 	  if(C->getValue().isOne()) {
                  llvm::errs() << "Ottimizzazione applicata: " << *BO << " -> " << *LHS << "\n";
    						  BO->replaceAllUsesWith(LHS);
                  BO->eraseFromParent();
                  Transformed = true;
                /* -- STRENGTH REDUCTION x*15 -> x -- */
    					  } else if(C->getValue().isPowerOf2()) {
                  auto *ShiftCount = ConstantInt::get(C->getType(), C->getValue().exactLogBase2());
                  auto *ShiftLeftOp = BinaryOperator::CreateShl(LHS, ShiftCount);
                  ShiftLeftOp->insertAfter(BO);
                  BO->replaceAllUsesWith(ShiftLeftOp);
                  BO->eraseFromParent();
                  Transformed = true;
                } else if((C->getValue()-1).isPowerOf2()) {
                  auto temp = LHS;
                  auto *ShiftCount = ConstantInt::get(C->getType(), (C->getValue()-1).exactLogBase2());
                  auto *ShiftLeftOp = BinaryOperator::CreateShl(LHS, ShiftCount);
                  ShiftLeftOp->insertAfter(BO);

                  auto *AdditionOp = BinaryOperator::CreateAdd(ShiftLeftOp, temp);
                  AdditionOp->insertAfter(ShiftLeftOp);
                  BO->replaceAllUsesWith(AdditionOp);
                  BO->eraseFromParent();
                  Transformed = true;
                } else if((C->getValue()+1).isPowerOf2()) {
                  auto temp = LHS;
                  auto *ShiftCount = ConstantInt::get(C->getType(), (C->getValue()+1).exactLogBase2());
                  auto *ShiftLeftOp = BinaryOperator::CreateShl(LHS, ShiftCount);
                  ShiftLeftOp->insertAfter(BO);

                  auto *SubtractOp = BinaryOperator::CreateSub(ShiftLeftOp, temp);
                  SubtractOp->insertAfter(ShiftLeftOp);
                  BO->replaceAllUsesWith(SubtractOp);
                  BO->eraseFromParent();
                  Transformed = true;
                }
              /* -- ALGEBRAIC IDENTITY 1*x -> x -- */
    				  } else if(auto *C = dyn_cast<ConstantInt>(LHS)) { 
                /* Se l'operando LHS di tipo ConstantInt è 1 allora si procede al replace */
    					  if(C->getValue().isOne()) {
                  llvm::errs() << "Ottimizzazione applicata: " << *BO << " -> " << *RHS << "\n";
    						  BO->replaceAllUsesWith(RHS);
                  BO->eraseFromParent();
    						  Transformed = true;
                /* -- STRENGTH REDUCTION 15*x -> x -- */
    					  } else if(C->getValue().isPowerOf2()) {
                  auto *ShiftCount = ConstantInt::get(C->getType(), C->getValue().exactLogBase2());
                  auto *ShiftLeftOp = BinaryOperator::CreateShl(RHS, ShiftCount);
                  ShiftLeftOp->insertAfter(BO);
                  BO->replaceAllUsesWith(ShiftLeftOp);
                  BO->eraseFromParent();
                  Transformed = true;
                } else if((C->getValue()-1).isPowerOf2()) {
                  auto temp = RHS;
                  auto *ShiftCount = ConstantInt::get(C->getType(), (C->getValue()-1).exactLogBase2());
                  auto *ShiftLeftOp = BinaryOperator::CreateShl(RHS, ShiftCount);
                  ShiftLeftOp->insertAfter(BO);

                  auto *AdditionOp = BinaryOperator::CreateAdd(ShiftLeftOp, temp);
                  AdditionOp->insertAfter(ShiftLeftOp);
                  BO->replaceAllUsesWith(AdditionOp);
                  BO->eraseFromParent();
                  Transformed = true;
                } else if((C->getValue()+1).isPowerOf2()) {
                  auto temp = RHS;
                  auto *ShiftCount = ConstantInt::get(C->getType(), (C->getValue()+1).exactLogBase2());
                  auto *ShiftLeftOp = BinaryOperator::CreateShl(RHS, ShiftCount);
                  ShiftLeftOp->insertAfter(BO);

                  auto *SubtractOp = BinaryOperator::CreateSub(ShiftLeftOp, temp);
                  SubtractOp->insertAfter(ShiftLeftOp);
                  BO->replaceAllUsesWith(SubtractOp);
                  BO->eraseFromParent();
                  Transformed = true;
                }
              }
    			  }
    		  } else if (BO->getOpcode() == Instruction::SDiv) {
              LHS = BO->getOperand(0);
              RHS = BO->getOperand(1);
          
              if(auto *C = dyn_cast<ConstantInt>(RHS)) {
                if(C->getValue().isOne()) {
                  BO->replaceAllUsesWith(LHS);
                  BO->eraseFromParent(); // Rimuovi l'istruzione morta
                  Transformed = true;
                } else if(C->getValue().isPowerOf2()) {
                  Constant *ShiftCount = ConstantInt::get(C->getType(), C->getValue().exactLogBase2());
                  auto *ShiftRight = BinaryOperator::CreateLShr(LHS, ShiftCount);
                  ShiftRight->insertAfter(BO);
                  BO->replaceAllUsesWith(ShiftRight);
                  BO->eraseFromParent();
                  Transformed = true;
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
}; // namespace


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
}