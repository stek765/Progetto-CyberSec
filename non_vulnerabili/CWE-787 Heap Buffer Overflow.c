/**
 * @file CWE-787 Heap Buffer Overflow.c
 * @brief FIX: Scrittura OOB su Heap risolta con controllo dimensione
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char *buffer = malloc(10);
    if (!buffer)
        return 1;

    if (argc > 1)
    {
        // FIX: Controlliamo se la lunghezza della stringa supera la dimensione allocata
        if (strlen(argv[1]) >= 10)
        {
            printf("Errore: Input troppo lungo per il buffer!\n");
        }
        else
        {
            strcpy(buffer, argv[1]);
            printf("Heap buffer (safe): %s\n", buffer);
        }
    }
    free(buffer);
    return 0;
}
