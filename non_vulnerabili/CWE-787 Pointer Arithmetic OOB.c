/**
 * @file CWE-787 Pointer Arithmetic OOB.c
 * @brief FIX: Offset validato prima dell'aritmetica
 *
 * Strumenti consigliati per verifica:
 * - Compilazione: gcc -o "build/CWE-787 Pointer Arithmetic OOB Patched" "non_vulnerabili/CWE-787 Pointer Arithmetic OOB.c"
 * - Analisi: Verificare output di errore per offset non validi
 */
#include <stdio.h>

int main()
{
    int array[3] = {10, 20, 30};
    int *ptr = array;
    int offset;

    printf("Offset puntatore: ");
    scanf("%d", &offset);

    // FIX: L'offset deve essere validato rispetto alla dimensione dell'array puntato
    if (offset >= 0 && offset < 3)
    {
        *(ptr + offset) = 999;
        printf("Scritto 999 all'offset %d (safe)\n", offset);
    }
    else
    {
        printf("Errore: Offset %d non valido!\n", offset);
    }

    return 0;
}
