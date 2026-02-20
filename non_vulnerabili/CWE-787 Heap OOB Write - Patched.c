/**
 * @file CWE-787 Heap OOB Write - Patched.c
 * @brief FIX per CWE-787: Strict Logic Check
 *
 * Descrizione Fix:
 * Il controllo sull'indice `numWidgets` è reso rigoroso. La scrittura
 * avviene solo se `numWidgets < MAX_NUM_WIDGETS`.
 *
 * Strumenti consigliati per verifica:
 * - Compilazione: gcc -no-pie -fno-stack-protector -z execstack -o "build/CWE-787 Heap OOB Write - Patched" "non_vulnerabili/CWE-787 Heap OOB Write - Patched.c"
 * - Test manuale: ./build/CWE-787\ Heap\ OOB\ Write\ -\ Patched 5 (Dovrebbe fallire gracefully)
 */

// SAFE Example 5: Out-of-bounds Write (Heap Corruption) Patched
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_NUM_WIDGETS 4

typedef struct
{
    int isAdmin;
    char name[64];
} AdminSession;

typedef struct Widget
{
    int x;
} Widget;

Widget **WidgetList;
AdminSession *admin;

void win()
{
    printf("This should not be reached!\n");
    char *args[] = {"/bin/sh", NULL};
    execve("/bin/sh", args, NULL);
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

    // PATCH: Allow write ONLY if strictly less than MAX
    if (numWidgets >= MAX_NUM_WIDGETS)
    {
        printf("Index OOB detected. Using max safe index.\n");
        // We do not return 1, but we prevent the write below
    }

    // FILL ARRAY
    for (i = 0; i < MAX_NUM_WIDGETS; i++)
    {
        WidgetList[i] = malloc(sizeof(Widget));
        WidgetList[i]->x = i;
    }

    // Write to the user supplied index, GUARDED
    if (numWidgets < MAX_NUM_WIDGETS)
    {
        WidgetList[numWidgets] = (Widget *)0x41414141;
        printf("Wrote to index %u\n", numWidgets);
    }
    else
    {
        printf("Validation blocked writing to %u\n", numWidgets);
    }

    printf("Is Admin? %d\n", admin->isAdmin);

    if (admin->isAdmin != 0)
    {
        win();
    }
    else
    {
        printf("Access Denied.\n");
    }

    return 0;
}
