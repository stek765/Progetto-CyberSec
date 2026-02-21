# Vulnerabilità e Fix: CWE-787 Out-of-bounds Write

Questo repository contiene esempi di codice vulnerabile alla scrittura fuori dai limiti (Out-of-bounds Write, CWE-787) e le relative versioni corrette (patch).

## Struttura del Repository

- `vulnerabili/`: Contiene i codici sorgente C affetti da vulnerabilità.
- `non_vulnerabili/`: Contiene i codici sorgente con le fix applicate.

## Guida alla Compilazione

Per scopi educativi e di testing delle vulnerabilità (exploit development), è necessario compilare i file vulnerabili disabilitando le protezioni moderne del compilatore (Stack Canary, NX, PIE, ASLR).

### Compilazione dei file Vulnerabili

Si consiglia di utilizzare `gcc` con i seguenti flag:

```bash
gcc -m32 -fno-stack-protector -z execstack -no-pie -o "build/NOME_ESECUTIBILE" "vulnerabili/NOME_FILE.c"
```

**Spiegazione dei Flag:**
*   `-m32`: Compila a 32-bit (opzionale, ma semplifica spesso lo studio della memoria e la creazione di shellcode).
*   `-fno-stack-protector`: Disabilita lo "Stack Canary", un valore casuale posto prima dell'indirizzo di ritorno per rilevare buffer overflow. Senza questo flag, è possibile sovrascrivere l'indirizzo di ritorno silenziosamente.
*   `-z execstack`: Rende lo stack eseguibile. Modernamente lo stack è "NX" (No-Execute) per prevenire l'esecuzione di shellcode iniettati. Questo flag permette l'esecuzione di codice arbitrario (RCE) se iniettato nello stack.
*   `-no-pie`: Disabilita "Position Independent Executable". Questo fa sì che il codice venga caricato ad indirizzi statici, rendendo più facile calcolare i salti (JMP) verso le funzioni o i gadget.

> **Nota:** Per simulare un ambiente reale, bisognerebbe disabilitare anche l'ASLR a livello di sistema operativo: `sudo sysctl -w kernel.randomize_va_space=0`.

### Compilazione dei file Fixati

I file fixati possono essere compilati normalmente, permettendo al compilatore di applicare le protezioni standard.

```bash
gcc -o "build/NOME_ESECUTIBILE_PATCHED" "non_vulnerabili/NOME_FILE.c"
```

---

## Analisi dei File

### 1. `CWE-787 Array Index OOB.c`

*   **Funzionamento del Programma:** Il programma crea una lista di 5 numeri. Ti chiede: "In quale posizione (indice) vuoi scrivere?" e "Che numero vuoi inserire?". Poi prende il numero che hai scelto e lo scrive nella posizione indicata, senza controllare se quella posizione esiste davvero nella lista.
*   **Vulnerabilità:**
    *   **Punto Preciso:** Riga 22: `numbers[index] = value;`.
    *   **Spiegazione Tecnica:** L'indice `index` viene letto da input (scanf) e utilizzato direttamente per accedere all'array senza alcuna verifica preliminare. In C, l'accesso agli array è semplice aritmetica dei puntatori (`base + index * size`). Se l'utente inserisce un indice negativo (es. -1) o superiore alla dimensione (es. 6), il programma scriverà in un'area di memoria adiacente all'array, potenzialmente sovrascrivendo variabili locali, il Saved Frame Pointer o il Return Address residenti sullo stack.
*   **Attacco:** **Arbitrary Write** (Scrittura Arbitraria). Un attaccante può fornire un indice negativo o molto grande per scrivere il valore in una zona di memoria sensibile al di fuori dell'array.
*   **Conseguenze:**
    *   **RCE (Remote Code Execution):** Sovrascrivendo l'indirizzo di ritorno (Return Address) salvato sullo stack con l'indirizzo di uno shellcode.
    *   **DoS (Denial of Service):** Scrivendo dati corrotti in variabili di controllo o puntatori critici, causando un crash (Segmentation Fault).
*   **Fix:** È stato introdotto un controllo stretto sui limiti (`bounds checking`): `if (index >= 0 && index < 5)`.

### 2. `CWE-787 Stack Buffer Overflow.c`

*   **Funzionamento del Programma:** Il programma prende il testo che scrivi quando lo avvii da terminale. Ha un piccolo spazio di memoria riservato (buffer) che può contenere solo 10 lettere. Copia tutto il tuo testo dentro questo spazio senza controllare quanto è lungo. Infine ristampa quello che ha copiato.
*   **Vulnerabilità:**
    *   **Punto Preciso:** Riga 16: `strcpy(buffer, argv[1]);`.
    *   **Spiegazione Tecnica:** La funzione `strcpy` copia la stringa sorgente byte per byte fino a incontrare il carattere terminatore nullo (`\0`), senza controllare se la destinazione è abbastanza grande. Poiché `buffer` è dichiarato di soli 10 byte, se `argv[1]` è più lungo, i byte in eccesso continueranno a essere scritti nello stack frame della funzione `main`, sovrascrivendo i dati salvati (tra cui EBP e EIP) che gestiscono il flusso del programma.
*   **Attacco:** **Stack Smashing / Buffer Overflow**. Se l'input supera i 10 byte, i dati in eccesso sovrascrivono le variabili adiacenti nello stack, il Base Pointer (EBP) e infine il Return Address (EIP).
*   **Conseguenze:**
    *   **RCE (Privilege Escalation):** L'attaccante inietta codice malevolo (shellcode) e devia il flusso di esecuzione per ottenerne il controllo (spesso ottenendo una shell root).
*   **Fix:** Sostituzione di `strcpy` con `strncpy`, specificando la dimensione massima del buffer (`sizeof(buffer) - 1`) e assicurando la terminazione della stringa con `\0`.

### 3. `CWE-787 Heap Buffer Overflow.c`

*   **Funzionamento del Programma:** Il programma chiede al computer una piccola area di memoria dinamica (Heap) grande 10 caratteri. Prende il testo che gli passi all'avvio e lo copia dentro questa area, poi lo stampa a video. Alla fine pulisce la memoria.
*   **Vulnerabilità:**
    *   **Punto Preciso:** Riga 16: `strcpy(buffer, argv[1]);`.
    *   **Spiegazione Tecnica:** Anche qui viene usata `strcpy` insicura. La differenza rispetto al caso precedente è che il buffer risiede nello Heap (memoria dinamica) e non sullo Stack. Scrivendo oltre i 10 byte allocati, si vanno a sovrascrivere i metadati che il gestore della memoria (allocator, es. `glibc malloc`) usa per tracciare i blocchi liberi e occupati (i cosiddetti "chunk headers").
*   **Attacco:** **Heap Overflow**. L'attaccante scrive oltre il blocco allocato, corrompendo i metadati dello Heap (chunk header) o i dati di oggetti adiacenti.
*   **Conseguenze:**
    *   **Heap Corruption:** Può portare a crash del programma (DoS).
    *   **RCE:** Tramite tecniche avanzate come "Unsafe Unlink" o sovrascrivendo puntatori a funzione presenti in strutture adiacenti nello heap.
*   **Fix:** Controllo preventivo della lunghezza dell'input (`strlen`) prima di effettuare la copia, oppure uso di versioni sicure di allocazione/copia.

### 4. `CWE-787 Pointer Arithmetic OOB.c`

*   **Funzionamento del Programma:** Il programma ha una lista di 3 numeri e un "puntatore" (un dito che indica l'inizio della lista). Ti chiede di quanto spostare questo dito (offset). Poi scrive il numero `999` nel punto esatto dove è arrivato il dito, ovunque esso sia finito.
*   **Vulnerabilità:**
    *   **Punto Preciso:** Riga 21: `*(ptr + offset) = 999;`.
    *   **Spiegazione Tecnica:** L'aritmetica dei puntatori in C non esegue controlli sui limiti. L'espressione `ptr + offset` calcola un indirizzo di memoria sommando `offset * sizeof(int)` all'indirizzo base di `ptr`. Poiché `offset` è controllato dall'utente e non validato, l'indirizzo risultante può puntare ovunque nello spazio di indirizzamento del processo (entro i limiti dei segmenti scrivibili), permettendo di scrivere `999` in posizioni arbitrarie.
*   **Attacco:** Simile all'Array Indexing, ma sfrutta l'aritmetica dei puntatori. In C, `*(ptr + offset)` accede alla memoria all'indirizzo `base + (offset * dimensione_tipo)`.
*   **Conseguenze:**
    *   **Information Leak:** Se usato in lettura, permette di leggere dati sensibili dalla memoria (es. chiavi crittografiche, password).
    *   **Code Execution:** Se usato in scrittura (come in questo file), permette di sovrascrivere funzioni o indirizzi di ritorno.
*   **Fix:** Validazione dell'offset per assicurarsi che `ptr + offset` punti ancora all'interno dell'area di memoria legittima.

### 5. `CWE-787 Off-by-one Error.c`

*   **Funzionamento del Programma:** Il programma ha uno spazio per 10 lettere. Usa un ciclo (un'operazione ripetuta) per riempire questo spazio con la lettera 'A'. Per sbaglio, conta da 0 fino a 10 incluso, facendo 11 passaggi invece di 10.
*   **Vulnerabilità:**
    *   **Punto Preciso:** Riga 17: `for (i = 0; i <= 10; i++)`.
    *   **Spiegazione Tecnica:** La condizione di terminazione del ciclo è `i <= 10`. Poiché l'array ha dimensione 10, gli indici validi vanno da 0 a 9. Quando `i` vale 10, la condizione `10 <= 10` è vera, quindi il ciclo esegue un'iterazione in più, scrivendo `buffer[10]` (l'11esimo byte). Questo singolo byte extra trabocca dal buffer e sovrascrive il byte adiacente in memoria (che sullo stack è spesso il byte meno significativo del Saved Frame Pointer).
*   **Attacco:** **Off-by-one**. Anche un singolo byte può essere devastante. Nello stack, questo byte è spesso il byte meno significativo del Saved Frame Pointer (EBP).
*   **Conseguenze:**
    *   **Alterazione del Flusso di Esecuzione:** La modifica involontaria anche di un solo byte critico in memoria (come un indirizzo di ritorno) impedisce al programma di proseguire correttamente. Questo costringe l'applicazione a deviare dal percorso previsto, eseguendo istruzioni diverse da quelle originali.
    *   **Esecuzione di Codice Arbitrario (RCE):** Sfruttando questa deviazione, un attaccante può prendere il controllo del processo e costringerlo ad eseguire comandi malevoli con gli stessi privilegi dell'utente che ha avviato il programma, compromettendo la sicurezza del sistema.
*   **Fix:** Correzione della condizione del ciclo in `i < 10`.
