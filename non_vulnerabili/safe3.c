// SAFE Example 3: strcpy with unchecked buffer size Patched
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void win() {
    printf("This should not be reached!\n");
    char *args[] = {"/bin/sh", NULL};
    execve("/bin/sh", args, NULL);
}

void host_lookup() {
    char hostname[64];
    printf("Enter hostname: ");
    fflush(stdout);
    
    // PATCH: read only sizeof(hostname) - 1 to leave room for null terminator if string
    // or just strict size limit
    read(0, hostname, sizeof(hostname)-1); 
    hostname[sizeof(hostname)-1] = '\0'; // Ensure null termination
    
    printf("Hostname: %s\n", hostname);
}

int main(int argc, char *argv[]) {
    host_lookup();
    return 0;
}
