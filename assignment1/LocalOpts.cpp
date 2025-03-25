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

  /* 
    Funzione che estrae il valore costante e la variabile indipendente da un'operazione binaria.  
    Se uno degli operandi è una costante intera, lo assegna a C (passaggio per riferimento) e assegna l'altro operando a Var.  
    Restituisce true se un operando costante è stato trovato, altrimenti false.  
  */
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
      return false;

    return true;
  }

  /* 
    Funzione che implementa il passo di ottimizzazione "Algebraic Identity" su un BasicBlock.
    Identifica e semplifica operazioni binarie ridondanti, come:
      - Addizioni con zero (x + 0)
      - Moltiplicazioni per uno (x * 1)
    Restituisce true dopo aver processato tutte le istruzioni nel blocco.
  */
  bool algebraicIdentity(BasicBlock &B)
  {
    ConstantInt *C;
	  Value *Var;

    for(auto It = B.begin(); It != B.end(); ) 
    {
      Instruction &I = *It++;

    	if(auto *BO = dyn_cast<BinaryOperator>(&I)) 
      {
        if(BO->getOpcode() != Instruction::Add && BO->getOpcode() != Instruction::Mul)
          continue;

        if(!getConstant(BO, C, Var))
          continue;

        if(BO->getOpcode() == Instruction::Add) 
        {
          /* -- ALGEBRAIC IDENTITY (x + 0) -- */
          if(C->getValue().isZero()) 
          {
            // errs() << "Ottimizzazione applicata: " << *BO << " -> " << *Var << "\n";
            BO->replaceAllUsesWith(Var);
          }          
        }
        else
        {
          if(C->getValue().isOne()) 
          {
            // errs() << "Ottimizzazione applicata: " << *BO << " -> " << *Var << "\n";
            BO->replaceAllUsesWith(Var);
          } 
        }
      }
    }
    return true;
  }

  /*
    Funzione che implementa il passo di ottimizzazione "Strength Reduction" a un BasicBlock.
    Identifica moltiplicazioni e divisioni per costanti e le sostituisce con operazioni più efficienti quando possibile:
      - Moltiplicazioni per potenze di due vengono sostituite con shift a sinistra.
      - Divisioni per potenze di due vengono sostituite con shift a destra.
      - Moltiplicazioni o divisioni per valori vicini a potenze di due vengono trasformate in combinazioni di shift e addizioni o sottrazioni.
    Restituisce true dopo aver processato tutte le istruzioni nel blocco.
  */
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
          /* -- STRENGTH REDUCTION DIV -- */
          if(C->getValue().isOne()) 
            BO->replaceAllUsesWith(Var);
            
          else if(C->getValue().isPowerOf2()) 
          {
            auto *ShiftCount = ConstantInt::get(C->getType(), C->getValue().exactLogBase2());
            auto *ShiftRightOp = BinaryOperator::CreateLShr(Var, ShiftCount);
            ShiftRightOp->insertAfter(BO);
            BO->replaceAllUsesWith(ShiftRightOp);
          }
          else if((C->getValue()-1).isPowerOf2())
          {
            Temp = C;
            auto *ShiftCount = ConstantInt::get(C->getType(), (C->getValue()-1).exactLogBase2());
            auto *ShiftRightOp = BinaryOperator::CreateLShr(Var, ShiftCount);
            ShiftRightOp->insertAfter(BO);

            auto *AdditionOp = BinaryOperator::CreateAdd(ShiftRightOp, Temp);
            AdditionOp->insertAfter(ShiftRightOp);
            BO->replaceAllUsesWith(AdditionOp);
          }
          else if((C->getValue()+1).isPowerOf2())
          {
            Temp = C;
            auto *ShiftCount = ConstantInt::get(C->getType(), (C->getValue()+1).exactLogBase2());
            auto *ShiftRightOp = BinaryOperator::CreateLShr(Var, ShiftCount);
            ShiftRightOp->insertAfter(BO);

            auto *SubtractOp = BinaryOperator::CreateSub(ShiftRightOp, Temp);
            SubtractOp->insertAfter(ShiftRightOp);
            BO->replaceAllUsesWith(SubtractOp);
          }
        }
      }
    }

    return true;
  }

  /*
    Funzione che implementa il passo di ottimizzazione "Multi Instruction" per eliminare operazioni aritmetiche inverse consecutive in un BasicBlock.
    Analizza le istruzioni alla ricerca di addizioni e sottrazioni che operano sulla stessa variabile con la stessa costante.
    Se trova una coppia di operazioni inverse (es. addizione seguita da sottrazione della stessa costante), elimina l'operazione ridondante.
    Restituisce true dopo aver processato tutte le istruzioni nel blocco.
  */
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
    
        // errs() << "Analizzando l'istruzione BO: " << *BO << "\n";
      
        if (BO->getOpcode() != Instruction::Add && BO->getOpcode() != Instruction::Sub)
          continue;

        Instruction::BinaryOps oppositeOpcode = (BO->getOpcode() == Instruction::Add) ? Instruction::Sub : Instruction::Add;

        // errs() << "Numero di user per I: " << I.getNumUses() << "\n";
      
        for (auto UI = I.user_begin(); UI != I.user_end(); ++UI) { 
          if (auto *casted = dyn_cast<Instruction>(*UI)){
            Instruction *user_instruction = casted;

            // errs() << "User trovata: " << *user_instruction << "\n";

            if(user_instruction->getOpcode() == oppositeOpcode)
            {
              errs() << "Trovata operazione opposta!\n";
              if (C != user_instruction->getOperand(1)) 
                // errs() << "Operandi non corrispondono, salto l'istruzione.\n";
                continue; 

              // errs() << "MATCH TROVATO! Sostituiamo " << *user_instruction << " con " << Var << "\n";
              user_instruction->replaceAllUsesWith(Var);
              // errs() << "Istruzione modificata con successo.\n";
            }
          }
        }  
      }
    }

    return true;
  }
  
  /*
    Funzione che si occupa dell'eliminazione del codice morto (Dead Code Elimination) su un BasicBlock.
    Questa ottimizzazione migliora l'efficienza del codice eliminando istruzioni inutili.
    Restituisce true dopo aver processato tutte le istruzioni nel blocco.
  */
  bool deadCodeElimination(BasicBlock &B){

    auto It = B.begin();
  
    while(It != B.end()){
      Instruction &I = *It;
  
      if ((I.hasNUses(0)) && (I.isBinaryOp()))
        It = I.eraseFromParent();
      else
        It++;
    }
  
    return true;
  }
  
  bool runOnFunction(Function &F) 
  {
    bool Transformed = false;

    for (auto Iter = F.begin(); Iter != F.end(); ++Iter) 
    {
      if (algebraicIdentity(*Iter)) 
        Transformed = true;
      if (strenghtReduction(*Iter)) 
        Transformed = true;
      if (multiInstruction(*Iter))
        Transformed = true;
      if (deadCodeElimination(*Iter))
        Transformed = true;
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
