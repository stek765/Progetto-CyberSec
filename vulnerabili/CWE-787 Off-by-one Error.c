/**
 * @file CWE-787 Off-by-one Error.c
 * @brief CWE-787: Scrittura di un byte oltre il limite (Off-by-one)
 */
#include <stdio.h>

int main()
{
    char buffer[10];
    int i;

    // VULNERABILITÀ: Il ciclo usa '<= 10' invece di '< 10', scrivendo all'indice 10 (l'11esimo byte)
    for (i = 0; i <= 10; i++)
    {
        buffer[i] = 'A';
    }

    printf("Finito.\n");
    return 0;
}
