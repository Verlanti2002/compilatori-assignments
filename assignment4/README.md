# Assignment 4: 

## Loop Fusion
### Introduzione
Trasformazione di ottimizzazione del compilatore che consiste nell'unire due o più cicli for (o strutture iterative equivalenti) adiacenti, con identico intervallo di iterazione e struttura di controllo equivalente, in un singolo ciclo combinato.

Lo scopo è ridurre il numero complessivo di iterazioni, minimizzare l'overhead di controllo e migliorare la località dei dati, favorendo così prestazioni più efficienti, soprattutto in presenza di accessi ripetuti a strutture dati comuni.

### Condizioni di applicabilità
Per poter applicare correttamente la trasformazione di **Loop Fusion**, devono essere soddisfatte le seguenti condizioni:  

1. **Adiacenza dei loop**  
   I cicli devono essere posizionati uno immediatamente dopo l'altro nel codice, senza istruzioni intermedie che impediscano la fusione.

2. **Equivalenza del controllo di flusso**  
   I cicli devono appartenere allo stesso contesto di esecuzione, ovvero non devono essere condizionati da blocchi `if` mutuamente esclusivi o da salti condizionali incompatibili.

3. **Stesso intervallo di iterazione**  
   I loop devono condividere la stessa variabile di induzione e gli stessi limiti di iterazione (inizio, fine e passo).

4. **Assenza di dipendenze che impediscano la fusione**  
   Non devono esserci:
   - **Data dependencies** tra operazioni nei due loop che ne vincolano l’ordine
   - **Loop-carried dependencies** in cui un’iterazione successiva del secondo loop dipende da risultati di un’iterazione precedente del primo loop

Il rispetto di queste condizioni garantisce che la trasformazione non alteri il comportamento semantico del programma.  

### Verifica delle Ottimizzazioni
Confrontiamo i file `Test.m2r.ll` (non ottimizzato) e `TestOpt.ll` (ottimizzato) per verificare che le ottimizzazioni siano state applicate correttamente.

#### Esempio di Codice IR
##### Test.m2r.ll (non ottimizzato)
```llvm
define dso_local void @test_all_properties_ok() #0 {
  %1 = call ptr @llvm.stacksave.p0()
  %2 = alloca i32, i64 10, align 16
  %3 = alloca i32, i64 10, align 16
  %4 = alloca i32, i64 10, align 16
  %5 = alloca i32, i64 10, align 16
  br label %6

6:                                                ; preds = %0, %13
  %indvars.iv7 = phi i64 [ 0, %0 ], [ %indvars.iv.next, %13 ]
  %7 = getelementptr inbounds i32, ptr %2, i64 %indvars.iv7
  %8 = load i32, ptr %7, align 4
  %9 = getelementptr inbounds i32, ptr %3, i64 %indvars.iv7
  %10 = load i32, ptr %9, align 4
  %11 = add nsw i32 %8, %10
  %12 = getelementptr inbounds i32, ptr %4, i64 %indvars.iv7
  store i32 %11, ptr %12, align 4
  br label %13

13:                                               ; preds = %6
  %indvars.iv.next = add nuw nsw i64 %indvars.iv7, 1
  %exitcond = icmp ne i64 %indvars.iv.next, 10
  br i1 %exitcond, label %6, label %14, !llvm.loop !6

14:                                               ; preds = %13
  br label %15

15:                                               ; preds = %14, %20
  %indvars.iv38 = phi i64 [ 0, %14 ], [ %indvars.iv.next4, %20 ]
  %16 = getelementptr inbounds i32, ptr %4, i64 %indvars.iv38
  %17 = load i32, ptr %16, align 4
  %18 = mul nsw i32 %17, 2
  %19 = getelementptr inbounds i32, ptr %5, i64 %indvars.iv38
  store i32 %18, ptr %19, align 4
  br label %20

20:                                               ; preds = %15
  %indvars.iv.next4 = add nuw nsw i64 %indvars.iv38, 1
  %exitcond6 = icmp ne i64 %indvars.iv.next4, 10
  br i1 %exitcond6, label %15, label %21, !llvm.loop !8

21:                                               ; preds = %20
  call void @llvm.stackrestore.p0(ptr %1)
  ret void
}
```

##### TestOpt.ll (ottimizzato)
```llvm
define dso_local void @test_all_properties_ok() #0 {
  %1 = call ptr @llvm.stacksave.p0()
  %2 = alloca i32, i64 10, align 16
  %3 = alloca i32, i64 10, align 16
  %4 = alloca i32, i64 10, align 16
  %5 = alloca i32, i64 10, align 16
  br label %6

6:                                                ; preds = %13, %0
  %indvars.iv7 = phi i64 [ 0, %0 ], [ %indvars.iv.next, %13 ]
  %7 = getelementptr inbounds i32, ptr %2, i64 %indvars.iv7
  %8 = load i32, ptr %7, align 4
  %9 = getelementptr inbounds i32, ptr %3, i64 %indvars.iv7
  %10 = load i32, ptr %9, align 4
  %11 = add nsw i32 %8, %10
  %12 = getelementptr inbounds i32, ptr %4, i64 %indvars.iv7
  store i32 %11, ptr %12, align 4
  br label %15

13:                                               ; preds = %15
  %indvars.iv.next = add nuw nsw i64 %indvars.iv7, 1
  %exitcond = icmp ne i64 %indvars.iv.next, 10
  br i1 %exitcond, label %6, label %14, !llvm.loop !6

14:                                               ; preds = %13
  br label %15

15:                                               ; preds = %6, %20, %14
  %16 = getelementptr inbounds i32, ptr %4, i64 %indvars.iv7
  %17 = load i32, ptr %16, align 4
  %18 = mul nsw i32 %17, 2
  %19 = getelementptr inbounds i32, ptr %5, i64 %indvars.iv7
  store i32 %18, ptr %19, align 4
  br label %13

20:                                               ; No predecessors!
  %indvars.iv.next4 = add nuw nsw i64 %indvars.iv7, 1
  %exitcond6 = icmp ne i64 %indvars.iv.next4, 10
  br i1 %exitcond6, label %15, label %21, !llvm.loop !8

21:                                               ; preds = %20
  call void @llvm.stackrestore.p0(ptr %1)
  ret void
}
```

### Conclusione
La Loop Fusion Optimization ha fuso con successo i due loop, semplificando e ottimizzando il codice IR.

## **Applicazione delle Ottimizzazioni**
Posizionarsi nella cartella `assignement4` e utilizzare `opt` per applicare le ottimizzazioni definite nel progetto.

```sh
opt-19 -S -passes='mem2reg,loop-simplify,indvars,loop-rotate' test/Test.ll -o test/Test.m2r.ll
opt-19 -S -load-pass-plugin=build/libLoopFusionOpts.so -passes='loop-fusion-opts' test/Test.m2r.ll -o test/TestOpt.ll
```
- `load-pass-plugin build/libLocalOpts.so` Indica dove è definita la libreria contenente il passo di ottimizzazione realizzato.
- `-passes` Specifica la pipeline di ottimizzazioni da applicare.
- `mem2reg` Permette di generare il file `Test.m2r.ll` rimuovendo le operazioni superflue di `load` e `store`.
- `loop-simplify` Permette di portare i loop in una forma canonica:
    - aggiunge un **preheader** al loop
    - assicura che il **loop header** abbia un solo predecessore fuori dal loop
    - rende il **latch** (blocco che torna al loop header) esplicito
- `indvars` Trasforma le variabili di induzione in una forma standard.
- `loop-rotate` Ristruttura il loop spostando il test di uscita alla fine del corpo, convertendo while-like loop in do-while-like loop.
- `-S` indica che deve essere prodotto un file LLVM (.ll).

Questi due comandi generano, il primo un file esente da operazioni superflue di `load` e `store` (non ottimizzato), il secondo invece il file `TestOpt.ll` con le ottimizzazioni applicate.