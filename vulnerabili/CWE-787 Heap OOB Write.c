/**
 * @file CWE-787 Heap OOB Write.c
 * @brief CWE-787: Out-of-bounds Write (Heap)
 *
 * Descrizione:
 * Scrittura Out-of-bounds nell'Heap dovuta a un errore di logica nella validazione dell'indice.
 * Permette di scrivere oltre l'array di puntatori WidgetList, corrompendo la struttura
 * AdminSession adiacente.
 *
 * Strumenti consigliati:
 * - Compilazione: gcc -no-pie -fno-stack-protector -z execstack -o "build/CWE-787 Heap OOB Write" "vulnerabili/CWE-787 Heap OOB Write.c"
 * - Analisi: GDB, ltrace (per vedere i malloc)
 */

// CWE-787 Example 5: Out-of-bounds Write (Heap Corruption / Logic Bypass)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_NUM_WIDGETS 4

typedef struct
{
    int isAdmin; // 0 = false, 1 = true
    char name[64];
} AdminSession;

typedef struct Widget
{
    int x;
} Widget;

// Globals to ensure layout for demonstration (though malloc order usually sufficient)
Widget **WidgetList;
AdminSession *admin;

Widget *InitializeWidget()
{
    Widget *w = malloc(sizeof(Widget));
    w->x = 0;
    return w;
}

int main(int argc, char *argv[])
{
    int i;
    unsigned int numWidgets;

    if (argc < 2)
    {
        printf("Usage: %s <num_widgets>\n", argv[0]);
        return 1;
    }

    numWidgets = atoi(argv[1]);

    // Setup Admin session
    WidgetList = (Widget **)malloc(MAX_NUM_WIDGETS * sizeof(Widget *));
    admin = (AdminSession *)malloc(sizeof(AdminSession));
    admin->isAdmin = 0;

    printf("Admin Address: %p\n", admin);
    printf("Is Admin? %d\n", admin->isAdmin);

    // Validation
    if ((numWidgets == 0) || (numWidgets > MAX_NUM_WIDGETS))
    {
        // Vulnerability: Logic error allows OOB write
    }

    // FILL ARRAY
    for (i = 0; i < MAX_NUM_WIDGETS; i++)
    {
        WidgetList[i] = InitializeWidget();
    }

    // VULNERABILITY:
    // User requested index to write a value to.
    // If user sends "4", we write to WidgetList[4], which overlaps with 'admin'.
    // We emulate a write operation.
    if (numWidgets >= 0)
    {
        printf("Writing MAGIC_VAL to index %d\n", numWidgets);
        // We write a pointer. (unsigned long)1 = 0x00...01
        // If this overwrites admin->isAdmin (which is int), it becomes 1.
        WidgetList[numWidgets] = (Widget *)1;
    }

    // CHECK ADMIN
    if (admin->isAdmin)
    {
        printf("Access Granted! Spawning Shell...\n");
        system("/bin/sh");
    }
    else
    {
        printf("Access Denied.\n");
    }

    return 0;
}
