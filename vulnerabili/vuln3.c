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
