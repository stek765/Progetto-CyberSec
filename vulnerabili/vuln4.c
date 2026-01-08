// CWE-787 Example 4: Encoding expansion causes overflow (Heap FP Overwrite)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_SIZE 8

typedef struct {
    char buf[32];      
    void (*func_ptr)(); // Target for overflow
} Data;

void win() {
    printf("Excellent! Exploited vuln4. Spawning shell...\n");
    char *args[] = {"/bin/sh", NULL};
    execve("/bin/sh", args, NULL);
}

void normal_op() {
    printf("Normal operation executed.\n");
}

void copy_input(const char *user_supplied_string, Data *d) {
    int i, dst_index;
    
    // Vulnerability: Check is too loose for the expansion
    if (strlen(user_supplied_string) > 100) { // Relaxed check to allow exploit demo
        printf("user string too long, die evil hacker!\n");
        exit(1);
    }
    
    dst_index = 0;
    for (i = 0; i < strlen(user_supplied_string); i++) {
        if ('&' == user_supplied_string[i]) {
            // Expands to 5 chars: &amp;
            d->buf[dst_index++] = '&';
            d->buf[dst_index++] = 'a';
            d->buf[dst_index++] = 'm';
            d->buf[dst_index++] = 'p';
            d->buf[dst_index++] = ';'; 
        } else {
            d->buf[dst_index++] = user_supplied_string[i];
        }
    }
    d->buf[dst_index] = '\0';
}

int main(int argc, char *argv[]) {
    // Allocate on heap to stimulate heap overflow
    Data *d = (Data*)malloc(sizeof(Data));
    d->func_ptr = normal_op;
    
    printf("Function pointer before: %p\n", d->func_ptr);
    printf("Enter input string: ");
    fflush(stdout);

    char input[512];
    int len = read(0, input, 510);
    if(len > 0) input[len] = '\0';
    // Remove newline if present
    if(len > 0 && input[len-1] == '\n') input[len-1] = '\0';

    copy_input(input, d);
    printf("Function pointer after:  %p\n", d->func_ptr);
    
    // Execute function pointer
    d->func_ptr();
    
    free(d);
    return 0;
}
