// CWE-787 Example 5: Out-of-bounds Write (Heap Corruption / Logic Bypass)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_NUM_WIDGETS 4

typedef struct {
    int isAdmin; // 0 = false, 1 = true
    char name[64];
} AdminSession;

typedef struct Widget { int x; } Widget;

// Globals to ensure layout for demonstration (though malloc order usually sufficient)
Widget **WidgetList;
AdminSession *admin;

Widget* InitializeWidget() { Widget* w = malloc(sizeof(Widget)); w->x = 0; return w; }

int main(int argc, char *argv[]) {
    int i;
    unsigned int numWidgets;

    if (argc < 2) {
        printf("Usage: %s <num_widgets>\n", argv[0]);
        return 1;
    }

    numWidgets = atoi(argv[1]);
    
    // Setup Admin session (Safe by default)
    // We allocate these adjacently (hopefully)
    WidgetList = (Widget **)malloc(MAX_NUM_WIDGETS * sizeof(Widget *));
    admin = (AdminSession*)malloc(sizeof(AdminSession));
    admin->isAdmin = 0;

    printf("Admin Address: %p\n", admin);
    printf("Is Admin? %d\n", admin->isAdmin);

    // Validation
    if ((numWidgets == 0) || (numWidgets > MAX_NUM_WIDGETS)) {
        // Vulnerable: we proceed anyway? No, let's say the check is off-by-one or ignored
        // Or we just allow writing to index 4?
        // Let's implement the specific OOB logic:
        // "Oops, I'll allow writing to numWidgets index because I think array is 1-based or something"
    }

    // FILL ARRAY
    for(i = 0; i < MAX_NUM_WIDGETS; i++) {
        WidgetList[i] = InitializeWidget();
    }

    // VULNERABILITY:
    // User requested index to write a value to.
    // If user sends "4", we write to WidgetList[4], which overlaps with 'admin'.
    // We emulate a write operation.
    if (numWidgets >= 0) {
        printf("Writing MAGIC_VAL to index %d\n", numWidgets);
        // We write a pointer. (unsigned long)1 = 0x00...01
        // If this overwrites admin->isAdmin (which is int), it becomes 1.
        WidgetList[numWidgets] = (Widget*)1; 
    }

    // CHECK ADMIN
    if (admin->isAdmin) {
        printf("Access Granted! Spawning Shell...\n");
        system("/bin/sh");
    } else {
        printf("Access Denied.\n");
    }

    return 0;
}
