/**
 * @file CWE-787 Array Index OOB.c
 * @brief CWE-787: Scrittura OOB tramite indice array non controllato
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

    // VULNERABILITÀ: Nessun controllo se index >= 5 o negativo. Scrive memoria arbitraria.
    numbers[index] = value;

    printf("Scritto.\n");
    return 0;
}
