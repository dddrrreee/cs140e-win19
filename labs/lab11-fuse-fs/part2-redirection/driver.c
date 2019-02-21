// need to control your pi-shell from your pi-FS so that pi-FS can fake out
// pi-shell into sending echo, reboot, program commands.
//
// we will do so by fork/execing the shell, and dup2'ing its stdin/stdout/stderr
// file descriptors.
// 
// write the simple code here first to make sure the code works.
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "echo-until.h"
#include "redirect.h"
#include "demand.h"

#include "shell-prompt.h"


static int 
send_reply(int rd_fd, int wr_fd, const char *cmd, const char *prompt) {
    fd_puts(wr_fd, cmd);
    return echo_until(rd_fd, prompt);
}

// run pi-shell in sub-process with pipes replaced for stdin/stdout.
int main(int argc, char *argv[]) {
    note("about to try: <%s>\n", argv[1]);

    int rd_fd, wr_fd;
    redir(&rd_fd, &wr_fd, argv[1]);

    // trivial first test.
    if(strcmp(argv[1], "./test-redirect") == 0) {
        note("trivial test\n");
        fd_puts(wr_fd, "hello"); 
        char c = 'h';
        int retv;
        while((retv = read(rd_fd, &c, 1)) == 1)
            fputc(c, stderr);
        if(retv < 0)
            sys_die(read, problem with child?);
    } else {
        note("fancier test\n");
        // sadly we need to sleep b/c of the bootloader delays.
        sleep(1);
        echo_until(rd_fd, prompt);
        if(!send_reply(rd_fd,wr_fd,"echo foo\n", prompt))
            error("should not fail\n");
        if(send_reply(rd_fd,wr_fd,"reboot\n", prompt) != 0)
            error("should have failed\n");
    }
    note("redirect: done\n");
    return 0;
}
