/**
 * @file CWE-787 Array Index OOB.c
 * @brief FIX: Indice array validato
 */
#include <stdio.h>

int main()
{
    int numbers[5] = {0};
    int index, value;

    printf("Indice (0-4): ");
    scanf("%d", &index);
    printf("Valore: ");
    scanf("%d", &value);

    // FIX: Verifica rigorosa dei limiti dell'array
    if (index >= 0 && index < 5)
    {
        numbers[index] = value;
        printf("Scritto correttamente.\n");
    }
    else
    {
        printf("Errore: Indice %d fuori dai limiti!\n", index);
    }

    return 0;
}
