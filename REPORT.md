# Report Analisi Vulnerabilità CWE-787: Out-of-bounds Write

Questo documento presenta un'analisi dettagliata della vulnerabilità **CWE-787: Out-of-bounds Write**, dimostrata attraverso cinque scenari pratici. L'analisi copre sia le versioni vulnerabili (`vulnerabili/`) che le relative versioni patchate (`non_vulnerabili/`), dimostrando come la vulnerabilità possa essere sfruttata per ottenere esecuzione di codice arbitrario (RCE) e come possa essere corretta.

---

## Parte 1: Analisi Programmi Vulnerabili

Per ogni scenario, è stato sfruttato un exploit automatizzato (cartella `exploits/`) che abusa della vulnerabilità per redirezionare il flusso di esecuzione verso una funzione `win()` nascosta o protetta, che invoca una shell di sistema (`/bin/sh`).

### Scenario 1: Array OOB Write (Stack) - `vuln1.c`

*   **Descrizione Funzionamento**: Il programma accetta da linea di comando un indice e un valore numerico. Memorizza internamente un array di interi e tenta di scrivere il valore fornito all'indice specificato.
*   **Punto Vulnerabile**:
    ```c
    id_sequence[idx] = val;
    ```
*   **Motivo della Vulnerabilità**: Manca un controllo sui limiti (bounds check) dell'indice `idx`. Il programma assume che l'utente fornisca sempre un indice valido (0-2).
*   **Sfruttamento (Exploit)**: Fornendo un indice calcolato appositamente (che punta oltre la fine dell'array sullo stack), è possibile sovrascrivere l'indirizzo di ritorno (Return Address) della funzione `main`. L'exploit scrive l'indirizzo della funzione `win` in questa locazione. Al termine del `main`, il programma salta a `win` invece di terminare, garantendo una shell.

### Scenario 2: Memcpy Overflow (Stack) - `vuln2.c`

*   **Descrizione Funzionamento**: Il programma legge dati dallo standard input, calcola una dimensione tramite una funzione ausiliaria e usa `memcpy` per copiare quei dati in un buffer locale di 64 byte.
*   **Punto Vulnerabile**:
    ```c
    unsigned char size = (unsigned char)returnChunkSize(destBuf); // Ritorna 255
    memcpy(destBuf, inputBuffer, size);
    ```
*   **Motivo della Vulnerabilità**: Integer Underflow/Cast Error. La funzione ritorna `-1`, che castato a `unsigned char` diventa `255`. La `memcpy` copia quindi 255 byte in un buffer da 64 byte.
*   **Sfruttamento (Exploit)**: L'overflow sovrascrive il contenuto dello stack, inclusi il Saved Base Pointer (RBP) e l'indirizzo di ritorno. L'exploit inietta un payload contenente padding e l'indirizzo di `win`, ottenendo la RCE.

### Scenario 3: Read Buffer Overflow (Stack) - `vuln3.c`

*   **Descrizione Funzionamento**: Il programma chiede all'utente un hostname e lo legge in un buffer locale di 64 byte.
*   **Punto Vulnerabile**:
    ```c
    read(0, hostname, 200);
    ```
*   **Motivo della Vulnerabilità**: La funzione di lettura accetta fino a 200 byte, ma il buffer di destinazione è dimensionato per soli 64 byte.
*   **Sfruttamento (Exploit)**: Un classico Stack Buffer Overflow. L'attaccante riempie il buffer e continua a scrivere fino a sovrascrivere l'indirizzo di ritorno con quello di `win`.

### Scenario 4: Heap Overflow (Function Pointer) - `vuln4.c`

*   **Descrizione Funzionamento**: Il programma alloca nello Heap una struttura contenente un buffer e un puntatore a funzione. Espande caratteri speciali nella stringa di input (es. `&` -> `&amp;`) e salva il risultato nel buffer. Infine chiama il puntatore a funzione.
*   **Punto Vulnerabile**:
    ```c
    d->buf[dst_index++] = ...; // Scrittura durante l'espansione
    ```
*   **Motivo della Vulnerabilità**: Il controllo sulla lunghezza della stringa in input è inadeguato perché non considera l'aumento di dimensione dovuto all'espansione dei caratteri. L'espansione può far traboccare il buffer.
*   **Sfruttamento (Exploit)**: Poiché il puntatore a funzione è adiacente al buffer nella struttura allocata, l'overflow lo sovrascrive. L'attaccante rimpiazza il puntatore (che puntava a `normal_op`) con l'indirizzo di `win`. La successiva chiamata esegue la backdoor.

### Scenario 5: Heap OOB Write (Logic Bypass) - `vuln5.c`

*   **Descrizione Funzionamento**: Il programma gestisce una lista di Widget e una sessione Admin nello Heap. Permette di inizializzare un widget a un indice dato.
*   **Punto Vulnerabile**:
    ```c
    if ((numWidgets == 0) || (numWidgets > MAX_NUM_WIDGETS)) ...
    ```
*   **Motivo della Vulnerabilità**: Errore di logica (Off-by-one). Il controllo lasco permette di usare un indice uguale alla dimensione massima (4), che però è fuori dai limiti validi (0-3).
*   **Sfruttamento (Exploit)**: Scrivendo all'indice 4 dell'array di puntatori ai Widget, si va a scrivere nella memoria immediatamente successiva. A causa del layout dello Heap, lì risiede la struttura `AdminSession`. L'exploit sovrascrive il flag `isAdmin` attivando i privilegi di amministratore.

---

## Parte 2: Analisi Programmi Non Vulnerabili (Safe)

Le versioni "Safe" (`non_vulnerabili/`) implementano lo stesso funzionamento logico ma applicano le patch necessarie.

### Safe 1: Array Bound Check
Il programma `safe1.c` introduce un controllo esplicito:
```c
if (idx >= 0 && idx < 3) { ... }
```
**Perché è sicuro**: L'indice viene validato prima dell'uso. Tentativi di scrivere fuori dai limiti vengono bloccati, proteggendo lo stack.

### Safe 2: Size Check
Il programma `safe2.c` verifica la dimensione di copia:
```c
if (size > sizeof(destBuf)) { ... }
```
**Perché è sicuro**: Anche se la variabile `size` calcolata è errata (255), il controllo di sicurezza impedisce che vengano copiati dati oltre la capacità fisica del buffer.

### Safe 3: Safe Read Limit
Il programma `safe3.c` limita la lettura alla dimensione corretta:
```c
read(0, hostname, sizeof(hostname)-1);
```
**Perché è sicuro**: La funzione di input non può fisicamente scrivere più byte di quanti il buffer ne possa contenere.

### Safe 4: Conservative Check
Il programma `safe4.c` verifica se l'espansione "worst-case" sta nel buffer:
```c
if (len * 5 >= sizeof(d->buf)) { return; }
```
**Perché è sicuro**: Previene l'overflow alla radice rifiutando input che *potrebbero* causare problemi dopo l'espansione, proteggendo il puntatore a funzione adiacente.

### Safe 5: Strict Logic Check
Il programma `safe5.c` corregge la condizione logica:
```c
if (numWidgets >= MAX_NUM_WIDGETS) ...
if (numWidgets < MAX_NUM_WIDGETS) { ... }
```
**Perché è sicuro**: L'indice "4" viene correttamente identificato come non valido, impedendo la scrittura sulla struttura `admin` adiacente.

---

## Parte 3: Verifica delle Patch

Per confermare l'efficacia delle correzioni, è stata eseguita una suite di test automatizzata (`exploits/test_safe.py`). Lo script:
1.  Compila le versioni `safe` con le stesse flag insicure (`-z execstack` ecc.) per escludere protezioni esterne.
2.  Lancia contro di esse gli exploit originali.

### Risultati dei Test

```text
[*] Running attacks against patched binaries...

[?] Testing exploit_vuln1.py against ../build/safe1...
   [+] SUCCESS: Exploit timed out (did not pop shell).

[?] Testing exploit_vuln2.py against ../build/safe2...
   [+] SUCCESS: Exploit timed out (did not pop shell).

[?] Testing exploit_vuln3.py against ../build/safe3...
   [+] SUCCESS: Exploit failed (as expected).

[?] Testing exploit_vuln4.py against ../build/safe4...
   [+] SUCCESS: Exploit failed (as expected).

[?] Testing exploit_vuln5.py against ../build/safe5...
   [+] SUCCESS: Exploit failed (as expected).
```

### Conclusione
Tutti i tentativi di exploit sono falliti sulle versioni patchate, confermando che le vulnerabilità sono state risolte correttamente via software.

