// trivial test program to read commands, print a shell prompt until we see
// "reboot"
#include <stdio.h>
#include <string.h>

#include "demand.h"
#include "shell-prompt.h"

int main(void) {
    while(1) {
        char buf[1024];
        fprintf(stderr, "%s", prompt);
        if(!fgets(buf, sizeof buf, stdin))
            error("fgets failed\n");
        if(strcmp(buf, "reboot\n") == 0) {
            fprintf(stderr, "saw a reboot! <%s> done\n", buf);
            exit(0);
        }
        fprintf(stderr, "got: <%s>\n", buf);
    }
    return 0;
}
