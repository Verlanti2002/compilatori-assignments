# Assignment 1: 

## 1. Algebraic Identity Optimization
### Introduzione
Dimostriamo l'ottimizzazione di espressioni algebriche identitarie nel codice LLVM IR.  
L'obiettivo è eliminare operazioni ridondanti come `x + 0` e `x * 1` per semplificare il codice generato.

### Verifica delle Ottimizzazioni
Confrontiamo i file `Test.ll` (non ottimizzato) e `TestOpt.ll` (ottimizzato) per verificare che le ottimizzazioni siano state applicate correttamente.

#### Esempio di Codice IR
##### Test.ll (non ottimizzato)
```llvm
%6 = add nsw i32 %5, 0  ; x + 0
%8 = mul nsw i32 %7, 1  ; x * 1
```

##### TestOpt.ll (ottimizzato)
```llvm
%5 = load i32, ptr %2, align 4
%7 = load i32, ptr %3, align 4
```
Le operazioni ridondanti sono state rimosse correttamente.

### Conclusione
Le ottimizzazioni algebriche hanno eliminato con successo le operazioni ridondanti, semplificando il codice IR.

## 2. Strength Reduction
### Introduzione
Per ottimizzare ulteriormente il codice, si applicano tecniche di Strength Reduction per sostituire moltiplicazioni e divisioni con operazioni di shifting equivalenti meno costose.

Esempi:
- `15 × x = x × 15` ⇒ (x << 4) – x
- `y = x / 8` ⇒ y = x >> 3

Per verificare l'effettivo funzionamento del passo di ottimizzazione dobbiamo ancora una volta analizzare il risultato di `Test.ll` e `TestOpt.ll`.

#### Test.ll (non ottimizzato)
```llvm
%9 = mul nsw i32 %8, 8  ; x * 8
%11 = mul nsw i32 %10, 15 ; x * 15
%13 = mul nsw i32 %12, 17 ; x * 17
%15 = mul nsw i32 4, %14 ; 4 * x
%17 = mul nsw i32 17, %16 ; 17 * x

%7 = sdiv i32 %6, 8  ; x / 8
%9 = sdiv i32 %8, 10 ; x / 10
%11 = sdiv i32 %10, 16 ; x / 16
```

#### TestOpt.ll (ottimizzato)
```llvm
%9 = shl i32 %8, 3  ; x << 3

%11 = shl i32 %10, 4  ; (x << 4) - x
%12 = sub i32 %11, %10

%14 = shl i32 %13, 4  ; (x << 4) + x
%15 = add i32 %14, %13

%17 = shl i32 %16, 2  ; x << 2

%19 = shl i32 %18, 4  ; (x << 4) + x
%20 = add i32 %19, %18

%7 = lshr i32 %6, 3  ; x >> 3
%9 = sdiv i32 %8, 10  ; x / 10 non cambia se non potenza di 2
%11 = lshr i32 %10, 4  ; x >> 4
```
Le operazioni sono state correttamente sostituite con operazioni bitwise più efficienti.

### Conclusione
Le tecniche di Strength Reduction hanno sostituito con successo le operazioni di moltiplicazione e divisione con operazioni shifting più efficienti, migliorando l'efficienza del codice generato.

## 3. Multi-Instruction Optimization
### Introduzione
Nell'ultimo passo applichiamo procedimenti di Multi-Instruction Optimization per sostituire sequenze di istruzioni ridondanti con istruzioni più semplici.

Esempio:
 - `a = b + 1;`
 - `c = a - 1;`
 - allora -> `c = b;`

Per verificare l'effettivo funzionamento del passo di ottimizzazione dobbiamo ancora una volta analizzare il risultato di `Test.ll` e `TestOpt.ll`.

#### Test.ll (non ottimizzato)
```llvm
%6 = add nsw i32 %5, 1
store i32 %6, ptr %3, align 4
%7 = load i32, ptr %3, align 4
%8 = sub nsw i32 %7, 1
```

In `Test.ll` compaiono una load e una store in quanto la compilazione con `clang -O0` produce un codice meno efficiente in modo da semplificare il debug.

#### TestOpt.ll (ottimizzato)
```llvm
%2 = add nsw i32 %0, 1
ret i32 %0
```

### Conclusione
Analizzando il risultato di `TestOpt.ll` possiamo determinare che l'ottimizzazione è avvenuta correttamente.
- `%2` contiene la variabile `a`;
- `%0` contiene la variabile `b`;
- Il valore da restituire, ovvero `c` che corrisponde a `b` viene returnato effettivamente senza la necessità di un'ulteriore istruzione.

## **Applicazione delle Ottimizzazioni**
Posizionarsi nella cartella `assignement1` e utilizzare `opt` per applicare le ottimizzazioni definite nel progetto.

```sh
opt-19 -load-pass-plugin build/libLocalOpts.so -passes="module(mem2reg,local-opts)" -S test/Test.ll -o test/TestOpt.ll
```
- `load-pass-plugin build/libLocalOpts.so` Indica dove è definita la libreria contenente il passo di ottimizzazione realizzato.
- `-passes` Specifica la pipeline di ottimizzazioni da applicare.
- `mem2reg` Permette di generare il file `TestOpt.ll` rimuovendo le operazioni superflue di `load` e `store`.
- `-S` indica che deve essere prodotto un file LLVM (.ll).

Questo comando genera un nuovo file `TestOpt.ll` con le ottimizzazioni applicate.