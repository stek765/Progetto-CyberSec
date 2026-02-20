/**
 * @file CWE-787 Array OOB Write - Patched.c
 * @brief FIX per CWE-787: Bounds Checking
 *
 * Descrizione Fix:
 * Prima di accedere all'array `id_sequence[idx]`, viene effettuato un controllo
 * esplicito (`if (idx >= 0 && idx < 3)`) per garantire che l'indice sia valido.
 *
 * Strumenti consigliati per verifica:
 * - Compilazione: gcc -no-pie -fno-stack-protector -z execstack -o "build/CWE-787 Array OOB Write - Patched" "non_vulnerabili/CWE-787 Array OOB Write - Patched.c"
 * - Test: test_safe.py (verifica crash/exploit falliti)
 */

// SAFE Example 1: Out-of-bounds array write Patched
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void win()
{
    printf("This should not be reached!\n");
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

    // PATCH: Bounds check
    if (idx >= 0 && idx < 3)
    {
        printf("Writing %ld to index %d\n", val, idx);
        id_sequence[idx] = val;
    }
    else
    {
        printf("Index out of bounds! Blocked.\n");
    }

    printf("Done\n");
    return 0;
}
