#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"

using namespace llvm;

namespace {


// New PM implementation
struct LocalOptsModulePass: PassInfoMixin<LocalOptsModulePass> 
{
  // Main entry point, takes IR unit to run the pass on (&F) and the
  // corresponding pass manager (to be queried if need be)
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &) 
  {
    bool Transformed = false;
    for(auto &F : M)
    {
      if (runOnFunction(F))      
        Transformed = true;
    }
    return Transformed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }

  bool getConstant(BinaryOperator *BO, ConstantInt *&C, Value *&Var)
  {
    if (auto *constant = dyn_cast<ConstantInt>(BO->getOperand(0))) 
    {
      C = constant;
      Var = BO->getOperand(1);
    }
    else if (auto *constant = dyn_cast<ConstantInt>(BO->getOperand(1))) 
    {
      C = constant;
      Var = BO->getOperand(0);
    }
    else 
    {
      return false;
    }
    return true;
  }

  bool algebraicIdentity(BasicBlock &B)
  {
    ConstantInt *C;
	  Value *Var;

    for(auto It = B.begin(); It != B.end(); ) 
    {
      Instruction &I = *It++;

      /* Verifica se l'operazione è di tipo binario */
    	if(auto *BO = dyn_cast<BinaryOperator>(&I)) 
      {
        /* Verifica se l'operazione binaria è un'addizione o una moltiplicazione */
        if(BO->getOpcode() != Instruction::Add && BO->getOpcode() != Instruction::Mul)
          continue;

        if(!getConstant(BO, C, Var))
          continue;

        /* Se l'operazione è un'addizione...*/
        if(BO->getOpcode() == Instruction::Add) 
        {
          /* -- ALGEBRAIC IDENTITY (x + 0) -- */
          if(C->getValue().isZero()) 
          {
            llvm::errs() << "Ottimizzazione applicata: " << *BO << " -> " << *Var << "\n";
            BO->replaceAllUsesWith(Var);
          }          
        }
        else
        {
          if(C->getValue().isOne()) 
          {
            llvm::errs() << "Ottimizzazione applicata: " << *BO << " -> " << *Var << "\n";
            BO->replaceAllUsesWith(Var);
          } 
        }
      }
    }
    return true;
  }

  bool strenghtReduction(BasicBlock &B)
  {
    ConstantInt *C;
	  Value *Var;
    Value *Temp;

    for(auto It = B.begin(); It != B.end(); ) 
    {
      Instruction &I = *It++;

      if(auto *BO = dyn_cast<BinaryOperator>(&I))
      {
        if(!getConstant(BO, C, Var))
        continue;

        if (BO->getOpcode() != Instruction::Mul && BO->getOpcode() != Instruction::SDiv)
          continue;

        if(BO->getOpcode() == Instruction::Mul )
        {
          if(C->getValue().isPowerOf2())
          {
            auto *ShiftCount = ConstantInt::get(C->getType(), C->getValue().exactLogBase2());
            auto *ShiftLeftOp = BinaryOperator::CreateShl(Var, ShiftCount);
            ShiftLeftOp->insertAfter(BO);
            BO->replaceAllUsesWith(ShiftLeftOp);
          }
          else if((C->getValue()-1).isPowerOf2()) 
          {
            Temp = C;
            auto *ShiftCount = ConstantInt::get(C->getType(), (C->getValue()-1).exactLogBase2());
            auto *ShiftLeftOp = BinaryOperator::CreateShl(Var, ShiftCount);
            ShiftLeftOp->insertAfter(BO);
  
            auto *AdditionOp = BinaryOperator::CreateAdd(ShiftLeftOp, Temp);
            AdditionOp->insertAfter(ShiftLeftOp);
            BO->replaceAllUsesWith(AdditionOp);
          }
          else if((C->getValue()+1).isPowerOf2()) 
          {
            Temp = C;
            auto *ShiftCount = ConstantInt::get(C->getType(), (C->getValue()+1).exactLogBase2());
            auto *ShiftLeftOp = BinaryOperator::CreateShl(Var, ShiftCount); 
            ShiftLeftOp->insertAfter(BO);
          
            auto *SubtractOp = BinaryOperator::CreateSub(ShiftLeftOp, Temp);
            SubtractOp->insertAfter(ShiftLeftOp);
            BO->replaceAllUsesWith(SubtractOp);
          }
        }
        else
        {
          if(C->getValue().isOne()) 
            BO->replaceAllUsesWith(Var);
            
          /* -- STRENGTH REDUCTION DIV -- */
          else if(C->getValue().isPowerOf2()) 
          {
            Constant *ShiftCount = ConstantInt::get(C->getType(), C->getValue().exactLogBase2());
            auto *ShiftRight = BinaryOperator::CreateLShr(Var, ShiftCount);
            ShiftRight->insertAfter(BO);
            BO->replaceAllUsesWith(ShiftRight);
          }
        }
      }
    }

    return true;
  }

  bool multiInstruction(BasicBlock &B)
  {
    ConstantInt *C;
	  Value *Var;

    for(auto It = B.begin(); It != B.end(); ) 
    {
      Instruction &I = *It++;

      if(auto *BO = dyn_cast<BinaryOperator>(&I))
      {
        if(!getConstant(BO, C, Var))
          continue;

        /* -- MULTI INSTRUCTIONS -- */
        if (!BO || BO->use_empty()) // Controllo necessario per saltare le istruzioni ottimizzate (eliminate)
          continue;
    
        // Stampa l'istruzione corrente che stai analizzando
        errs() << "Analizzando l'istruzione BO: " << *BO << "\n";
      
        if (BO->getOpcode() != Instruction::Add && BO->getOpcode() != Instruction::Sub)
          continue;

        Instruction::BinaryOps oppositeOpcode = (BO->getOpcode() == Instruction::Add) ? Instruction::Sub : Instruction::Add;

        errs() << "Numero di user per I: " << I.getNumUses() << "\n";
      
        for (auto UI = I.user_begin(); UI != I.user_end(); ++UI) { 
          if (auto *casted = dyn_cast<Instruction>(*UI)){
            Instruction *user_instruction = casted;

            // Stampa il "user" che stai analizzando
            errs() << "User trovata: " << *user_instruction << "\n";

            if(user_instruction->getOpcode() == oppositeOpcode)
            {
              errs() << "Trovata operazione opposta!\n";
              if (C != user_instruction->getOperand(1)) 
                //errs() << "Operandi non corrispondono, salto l'istruzione.\n";
                continue; 

              errs() << "MATCH TROVATO! Sostituiamo " << *user_instruction << " con " << Var << "\n";
              user_instruction->replaceAllUsesWith(Var);
              // Stampa di conferma della sostituzione
              errs() << "Istruzione modificata con successo.\n";
            }
          }
        }  
      }
    }
    return true;
  }
    
  bool deadCodeElimination(BasicBlock &B){

    auto It = B.begin();
  
    while(It != B.end()){
      Instruction &I = *It;
  
      if ( (I.hasNUses(0)) && (I.isBinaryOp()) ){
        It = I.eraseFromParent();
      }
      else{
        It++;
      }
    }
  
    return true;
  }
  
  bool runOnFunction(Function &F) 
  {
    bool Transformed = false;

    /* Iterates on BasicBlocks */
    for (auto Iter = F.begin(); Iter != F.end(); ++Iter) 
    {
      if (algebraicIdentity(*Iter)) 
      {
        Transformed = true;
      }
      if (strenghtReduction(*Iter)) 
      {
        Transformed = true;
      }
      if (multiInstruction(*Iter)){
        Transformed = true;
      }
      if (deadCodeElimination(*Iter)){
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
  return {LLVM_PLUGIN_API_VERSION, "LocalOptsModulePass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "local-opts") {
                    MPM.addPass(LocalOptsModulePass());
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
