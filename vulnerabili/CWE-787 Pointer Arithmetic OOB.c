/**
 * @file CWE-787 Pointer Arithmetic OOB.c
 * @brief CWE-787: Scrittura OOB tramite aritmetica dei puntatori
 */
#include <stdio.h>

int main()
{
    int array[3] = {10, 20, 30};
    int *ptr = array;
    int offset;

    printf("Offset puntatore: ");
    scanf("%d", &offset);

    // VULNERABILITÀ: L'offset aggiunto al puntatore può puntare fuori dall'array
    *(ptr + offset) = 999; // scrive su Array[offset]

    printf("Scritto 999 all'offset %d\n", offset);
    return 0;
}
