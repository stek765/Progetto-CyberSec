/**
 * @file CWE-787 Off-by-one Error.c
 * @brief FIX: Loop condition corretta
 */
#include <stdio.h>

int main()
{
    char buffer[10];
    int i;

    // FIX: Condizione del ciclo corretta da '<=' a '<' per fermarsi prima dell'11esimo elemento
    for (i = 0; i < 10; i++)
    {
        buffer[i] = 'A';
    }

    printf("Finito (safe).\n");
    return 0;
}
