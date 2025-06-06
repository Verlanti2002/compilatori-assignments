# Compilatori-Assignments
Repository contenente la risoluzione degli Assignments del corso di Compilatori

## Configurazione dell'environment
Installazione degli strumenti necessari per la compilazione ed esecuzione dei passi di ottimizzazione.

## Requisiti
Prima di iniziare, assicurarsi di avere installato LLVM 19 e gli strumenti necessari.

### **Linux (Ubuntu/Debian)**
Installare LLVM 19 con il seguente comando:
```sh
sudo apt-get install -y llvm-19 llvm-19-dev llvm-19-tools clang-19
```

### **Windows (Usando Chocolatey)**
Se si utilizza Windows, si installi LLVM con:
```powershell
choco install -y llvm
```

Verificare l'installazione con:
```sh
llvm-config-19 --version
clang-19 --version
opt-19 --version
```
## **Scaricando i file sorgenti**
Un altro metodo sarebbe quello di scaricare direttamente i [file sorgenti](https://github.com/llvm/llvm-project/releases/tag/llvmorg-19.1.7).

## **Configurazione dell'ambiente**
Prima di compilare, è necessario impostare la variabile d'ambiente `LLVM_DIR`.

### **Linux**
```sh
export LLVM_DIR=<installation/dir/of/llvm-19>/bin
```

### **Windows (PowerShell)**
```powershell
$env:LLVM_DIR="<installation/dir/of/llvm-19>/bin"
```

## **Compilazione del progetto**
Posizionarsi all'interno della cartella `assignment1`.
Seguire questi passi per compilare:

1. Crea una directory di build:
   ```sh
   mkdir build
   cd build
   ```

2. Esegui **CMake** per configurare il progetto:
   ```sh
   cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR ..
   ```

3. Compila il progetto con **make**:
   ```sh
   make
   ```

Se la compilazione ha successo, otterrai l'insieme di passi di ottimizzazione sotto forma di libreria (`libLocalOpts.so`).

## **Generazione del Codice LLVM IR**
Posizionarsi all'interno della cartella dell'assignment in analisi e utilizzare `clang` per generare l'LLVM IR del file di test (`Test.c`).

```sh
clang-19 -emit-llvm -S -O0 -Xclang -disable-O0-optnone -c test/Test.c -o test/Test.ll
```
- L'opzione `-O0` disabilita le ottimizzazioni per ottenere un codice IR non ottimizzato.
- L'opzione `-emit-llvm` permette di generare codice intermedio LLVM invece di un eseguibile nativo.
- `-S` Permette di generare codice assembly invece che codice binario.
- `-Xclang  -disable-O0-optnone` Permette di generare il file `Test.ll` rimuovendo le operazioni superflue di `load` e `store`.
- `-c` Compila il codice producendo un file intermedio.
- `-o Test.ll` Specifica il nome dell'output.

# **Autori**
[![Alessandro Verlanti](https://img.shields.io/badge/GitHub-Verlanti2002-blue?logo=github)](https://github.com/Verlanti2002)
[![Giacomo Paltrinieri](https://img.shields.io/badge/GitHub-GiacomoPaltrinieri-blue?logo=github)](https://github.com/GiacomoPaltrinieri)
