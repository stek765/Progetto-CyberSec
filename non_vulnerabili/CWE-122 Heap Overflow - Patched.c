/**
 * @file CWE-122 Heap Overflow - Patched.c
 * @brief FIX per CWE-122: Output Buffer Calculation
 *
 * Descrizione Fix:
 * Prima di elaborare la stringa, viene calcolata la dimensione massima
 * possibile dell'espansione (Worst case: 5x). Se eccede il buffer, l'operazione è bloccata.
 *
 * Strumenti consigliati per verifica:
 * - Compilazione: gcc -no-pie -fno-stack-protector -z execstack -o "build/CWE-122 Heap Overflow - Patched" "non_vulnerabili/CWE-122 Heap Overflow - Patched.c"
 * - Analisi: Valgrind per verificare assenza di heap overflow
 */

// SAFE Example 4: Encoding expansion causes overflow Patched
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_SIZE 8

typedef struct
{
    char buf[32];
    void (*func_ptr)();
} Data;

void win()
{
    printf("This should not be reached!\n");
    char *args[] = {"/bin/sh", NULL};
    execve("/bin/sh", args, NULL);
}

void normal_op()
{
    printf("Normal operation executed.\n");
}

void copy_input(const char *user_supplied_string, Data *d)
{
    int i, dst_index;
    int len = strlen(user_supplied_string);

    // PATCH: Conservative check. Worst case expansion is 5x.
    if (len * 5 >= sizeof(d->buf))
    {
        printf("Input too long! Blocked.\n");
        return;
    }

    dst_index = 0;
    for (i = 0; i < len; i++)
    {
        if ('&' == user_supplied_string[i])
        {
            d->buf[dst_index++] = '&';
            d->buf[dst_index++] = 'a';
            d->buf[dst_index++] = 'm';
            d->buf[dst_index++] = 'p';
            d->buf[dst_index++] = ';';
        }
        else
        {
            d->buf[dst_index++] = user_supplied_string[i];
        }
    }
    d->buf[dst_index] = '\0';
}

int main(int argc, char *argv[])
{
    Data *d = (Data *)malloc(sizeof(Data));
    d->func_ptr = normal_op;

    if (argc < 1)
    { // Removed argv constraint since we read from stdin
      // return 1;
    }

    printf("Function pointer before: %p\n", d->func_ptr);
    printf("Enter input string: ");
    fflush(stdout);

    char input[512];
    int len = read(0, input, 510);
    if (len > 0)
        input[len] = '\0';
    // Remove newline if present
    if (len > 0 && input[len - 1] == '\n')
        input[len - 1] = '\0';

    copy_input(input, d);

    printf("Function pointer after:  %p\n", d->func_ptr);
    d->func_ptr();
    free(d);
    return 0;
}
