/**
 * @file CWE-120 Memcpy Overflow.c
 * @brief CWE-120: Buffer Copy without Checking Size of Input ('Classic Buffer Overflow')
 *
 * Descrizione:
 * Utilizzo di memcpy con una dimensione calcolata in modo errato (type casting error).
 * La funzione 'returnChunkSize' ritorna -1, che viene interpretato come 255 (unsigned char),
 * causando un overflow del buffer di destinazione (64 byte).
 *
 * Strumenti consigliati:
 * - Compilazione: gcc -no-pie -fno-stack-protector -z execstack -o "build/CWE-120 Memcpy Overflow" "vulnerabili/CWE-120 Memcpy Overflow.c"
 * - Analisi dinamica: Valgrind, AddressSanitizer
 */

// CWE-787 Example 2: memcpy with unchecked size (Ret2Win)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void win()
{
    printf("Excellent! Exploited vuln2. Spawning shell...\n");
    char *args[] = {"/bin/sh", NULL};
    execve("/bin/sh", args, NULL);
}

char returnChunkSize(void *ptr)
{
    // Returns -1, which casts to 255 as char/unsigned char
    return -1;
}

char inputBuffer[256];

int main(int argc, char *argv[])
{
    printf("Reading from stdin...\n");
    // Read from Stdin to allow Null bytes in payload
    ssize_t bytesRead = read(0, inputBuffer, 256);
    if (bytesRead < 0)
        return 1;

    char destBuf[64]; // Small buffer
    // Vulnerability: returnChunkSize returns -1 (0xFF = 255).
    unsigned char size = (unsigned char)returnChunkSize(destBuf);

    // 255 is bigger than 64 -> Overflow
    printf("Copying %d bytes...\n", size);

    memcpy(destBuf, inputBuffer, size);

    printf("Done\n");
    return 0;
}
