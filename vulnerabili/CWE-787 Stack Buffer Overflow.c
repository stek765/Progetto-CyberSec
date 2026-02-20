/**
 * @file CWE-787 Stack Buffer Overflow.c
 * @brief CWE-787: Scrittura OOB su Stack via strcpy
 */
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char buffer[10];

    if (argc > 1)
    {
        // VULNERABILITÀ: strcpy non controlla la dimensione, sovrascrive lo stack se argv[1] > 10 byte
        strcpy(buffer, argv[1]);
        printf("Hai scritto: %s\n", buffer);
    }
    return 0;
}
