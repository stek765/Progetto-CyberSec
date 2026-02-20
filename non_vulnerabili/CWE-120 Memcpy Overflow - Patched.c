/**
 * @file CWE-120 Memcpy Overflow - Patched.c
 * @brief FIX per CWE-120: Validate Size
 *
 * Descrizione Fix:
 * La dimensione da copiare (`size`) viene confrontata con la dimensione del buffer
 * di destinazione (`sizeof(destBuf)`). Se è maggiore, viene troncata o bloccata.
 *
 * Strumenti consigliati per verifica:
 * - Compilazione: gcc -no-pie -fno-stack-protector -z execstack -o "build/CWE-120 Memcpy Overflow - Patched" "non_vulnerabili/CWE-120 Memcpy Overflow - Patched.c"
 * - Analisi: Valgrind non rileverà "Invalid Write"
 */

// SAFE Example 2: memcpy with unchecked size Patched
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void win()
{
    printf("This should not be reached!\n");
    char *args[] = {"/bin/sh", NULL};
    execve("/bin/sh", args, NULL);
}

char returnChunkSize(void *ptr)
{
    // Returns -1, which casts to 255 as char/unsigned char
    return -1;
}

// Global to match vuln2 structure
char inputBuffer[256];

int main(int argc, char *argv[])
{
    printf("Reading from stdin...\n");
    // Read from Stdin
    ssize_t bytesRead = read(0, inputBuffer, 256);
    if (bytesRead < 0)
        return 1;

    char destBuf[64]; // Small buffer
    unsigned char size = (unsigned char)returnChunkSize(destBuf);

    printf("Copying %d bytes...\n", size);

    // PATCH: Check size against destination
    if (size > sizeof(destBuf))
    {
        printf("Error: Size %d too large for buffer %lu. Truncating.\n", size, sizeof(destBuf));
        memcpy(destBuf, inputBuffer, sizeof(destBuf));
    }
    else
    {
        memcpy(destBuf, inputBuffer, size);
    }

    printf("Done\n");
    return 0;
}
