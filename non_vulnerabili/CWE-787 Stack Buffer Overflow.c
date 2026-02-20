/**
 * @file CWE-787 Stack Buffer Overflow.c
 * @brief FIX: Scrittura OOB su Stack risolta con strncpy
 */
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char buffer[10];

    if (argc > 1)
    {
        // FIX: strncpy limita la copia a sizeof(buffer) - 1 per lasciare spazio al terminatore
        strncpy(buffer, argv[1], sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0'; // Assicura null-termination
        printf("Hai scritto (safe): %s\n", buffer);
    }
    return 0;
}
