/**
 * @file CWE-122 Heap Overflow.c
 * @brief CWE-122: Heap-based Buffer Overflow
 *
 * Descrizione:
 * Overflow nell'Heap causato dall'espansione della stringa (Encoding Expansion).
 * Simula una codifica HTML (es: testo in grassetto -> <b> codificato html -> &lt;b&gt;)
 *
 * Un controllo di lunghezza insicuro non tiene conto che il carattere '&' viene espanso
 * in 5 caratteri ("&amp;"), permettendo di sovrascrivere un puntatore a funzione adiacente.
 *
 * Strumenti consigliati:
 * - Compilazione: gcc -no-pie -fno-stack-protector -z execstack -o "build/CWE-122 Heap Overflow" "vulnerabili/CWE-122 Heap Overflow.c"
 * - Analisi Heap: GDB (vis, heap commands), Valgrind
 */

// CWE-787 Example 4: Encoding expansion causes overflow (Heap FP Overwrite)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_SIZE 8

typedef struct
{
    char buf[32];
    void (*func_ptr)(); // Target for overflow
} Data;

void win()
{
    printf("Excellent! Exploited vuln4. Spawning shell...\n");
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

    // Vulnerability: Check is too loose for the expansion
    if (strlen(user_supplied_string) > 100)
    {
        printf("user string too long, die evil hacker!\n");
        exit(1);
    }

    dst_index = 0;
    for (i = 0; i < strlen(user_supplied_string); i++)
    {
        if ('&' == user_supplied_string[i])
        {
            // Expands to 5 chars: &amp; // Vulnerability: this can overflow buf
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

    // Execute function pointer
    d->func_ptr();

    free(d);
    return 0;
}
