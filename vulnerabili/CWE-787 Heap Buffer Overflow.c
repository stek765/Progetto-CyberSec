/**
 * @file CWE-787 Heap Buffer Overflow.c
 * @brief CWE-787: Scrittura OOB su Heap
 *
 * Strumenti consigliati per verifica:
 * - Compilazione: gcc -no-pie -fno-stack-protector -z execstack -g -o "build/CWE-787 Heap Buffer Overflow" "vulnerabili/CWE-787 Heap Buffer Overflow.c"
 * - Analisi: Valgrind (memcheck), ltrace
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char *buffer = malloc(10);

    if (argc > 1)
    {
        // VULNERABILITÀ: strcpy scrive oltre la memoria allocata se l'input è troppo lungo
        strcpy(buffer, argv[1]);
        printf("Heap buffer: %s\n", buffer);
    }
    free(buffer);
    return 0;
}
