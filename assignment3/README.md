# Assignment 3: 

## Loop Invariant Code Motion Optimization
### Introduzione
Tecnica di ottimizzazione utilizzata per identificare ed estrarre espressioni invarianti dai loop, spostandole all'esterno del ciclo.  
L'obiettivo è eliminare i ricalcoli inutili di espressioni che producono sempre lo stesso risultato ad ogni iterazione, come nel caso di valori costanti, operazioni aritmetiche o chiamate a determinate funzioni.

### Verifica delle Ottimizzazioni
Confrontiamo i file `Test.m2r.ll` (non ottimizzato) e `TestOpt.ll` (ottimizzato) per verificare che le ottimizzazioni siano state applicate correttamente.

#### Esempio di Codice IR
##### Test.m2r.ll (non ottimizzato)
```llvm
define dso_local void @test() #0 {
  br label %1

1:                                                ; preds = %6, %0
  %.0 = phi i32 [ 0, %0 ], [ %5, %6 ]
  %2 = add nsw i32 5, 10
  %3 = add nsw i32 %2, %.0
  %4 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %3)
  %5 = add nsw i32 %.0, 1
  br label %6

6:                                                ; preds = %1
  %7 = icmp slt i32 %5, 10
  br i1 %7, label %1, label %8, !llvm.loop !6

8:                                                ; preds = %6
  ret void
}
```

##### TestOpt.ll (ottimizzato)
```llvm
define dso_local void @test() #0 {
  %1 = add nsw i32 5, 10
  br label %2

2:                                                ; preds = %6, %0
  %.0 = phi i32 [ 0, %0 ], [ %5, %6 ]
  %3 = add nsw i32 %1, %.0
  %4 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %3)
  %5 = add nsw i32 %.0, 1
  br label %6

6:                                                ; preds = %2
  %7 = icmp slt i32 %5, 10
  br i1 %7, label %2, label %8, !llvm.loop !6

8:                                                ; preds = %6
  ret void
}
```
Le operazioni invarianti sono state spostate correttamente.

### Conclusione
La LICM Optimization ha spostato con successo le operazioni invarianti presenti nel loop, semplificando e ottimizzando il codice IR.

## **Applicazione delle Ottimizzazioni**
Posizionarsi nella cartella `assignement3` e utilizzare `opt` per applicare le ottimizzazioni definite nel progetto.

```sh
opt-19 -S -passes='mem2reg,loop-simplify' test/Test.ll -o test/Test.m2r.ll
opt-19 -S -load-pass-plugin=build/libLoopOpts.so -passes=’loop-opts’ test/Test.m2r.ll -o test/TestOpt.ll
```
- `load-pass-plugin build/libLocalOpts.so` Indica dove è definita la libreria contenente il passo di ottimizzazione realizzato.
- `-passes` Specifica la pipeline di ottimizzazioni da applicare.
- `mem2reg` Permette di generare il file `Test.m2r.ll` rimuovendo le operazioni superflue di `load` e `store`.
- `-S` indica che deve essere prodotto un file LLVM (.ll).

Questi due comandi generano, il primo un file esente da operazioni superflue di `load` e `store` (non ottimizzato), il secondo invece il file `TestOpt.ll` con le ottimizzazioni applicate.