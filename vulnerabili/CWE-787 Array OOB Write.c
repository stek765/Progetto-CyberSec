/**
 * @file CWE-787 Array OOB Write.c
 * @brief CWE-787: Out-of-bounds Write
 *
 * Descrizione:
 * Scrittura fuori dai limiti di un array (Array Out-of-bounds Write).
 * L'indice fornito dall'utente non viene verificato prima di scrivere nell'array,
 * permettendo di sovrascrivere indirizzi di memoria adiacenti (es. indirizzo di ritorno).
 *
 * Strumenti consigliati:
 * - Compilazione: gcc -no-pie -fno-stack-protector -z execstack -o "build/CWE-787 Array OOB Write" "vulnerabili/CWE-787 Array OOB Write.c"
 * - Analisi dinamica: Valgrind (memcheck), GDB
 * - Analisi statica: Cppcheck
 */

// CWE-787 Example 1: Out-of-bounds array write (Ret2Win)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void win()
{
    printf("Excellent! Exploited vuln1. Spawning shell...\n");
    char *args[] = {"/bin/sh", NULL};
    execve("/bin/sh", args, NULL);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s <index> <value_as_long>\n", argv[0]);
        return 0;
    }

    int idx = atoi(argv[1]);
    long val = strtol(argv[2], NULL, 0);

    long id_sequence[3];
    id_sequence[0] = 123;
    id_sequence[1] = 234;
    id_sequence[2] = 345;

    printf("Writing %ld to index %d\n", val, idx);
    // VULNERABILITY: No check if idx is within 0-2
    id_sequence[idx] = val;

    printf("Done\n");
    return 0;
}
