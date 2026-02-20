/**
 * @file CWE-121 Read Buffer Overflow.c
 * @brief CWE-121: Stack-based Buffer Overflow
 *
 * Descrizione:
 * Classico Stack Buffer Overflow tramite la funzione 'read'.
 * Il programma legge 200 byte in un buffer di soli 64 byte, sovrascrivendo
 * il saved return address sullo stack.
 *
 * Strumenti consigliati:
 * - Compilazione: gcc -no-pie -fno-stack-protector -z execstack -o "build/CWE-121 Read Buffer Overflow" "vulnerabili/CWE-121 Read Buffer Overflow.c"
 * - Analisi: GDB (pattern create/offset), Checksec
 */

// CWE-787 Example 3: strcpy with unchecked buffer size (Ret2Win)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void win()
{
    printf("Excellent! Exploited vuln3. Spawning shell...\n");
    char *args[] = {"/bin/sh", NULL};
    execve("/bin/sh", args, NULL);
}

void host_lookup()
{
    char hostname[64];
    printf("Enter hostname: ");
    fflush(stdout);
    // Vulnerability: we read more than 64 bytes
    read(0, hostname, 200);

    printf("Hostname: %s\n", hostname);
}

int main(int argc, char *argv[])
{
    host_lookup();
    return 0;
}
