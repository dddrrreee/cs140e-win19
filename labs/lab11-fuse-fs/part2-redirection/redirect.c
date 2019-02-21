#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "redirect.h"
#include "demand.h"

// redir:
//  fork/exec <pi_process>
// return two file descripts:
//  rd_fd = the file descriptor the parent reads from to see the child's
//      stdout/stderr output.
//  wr_fd = the file descriptor the parent writes to in order to write data
//      to the child's stdin.
//
// you will override the child's stdin/stdout/stderr using pipes and dup2.
//      recall: pipe[0] = read in, pipe[1] = write end.
int redir(int *rd_fd, int *wr_fd, char * const pi_process) {
    // make sure you close all the fd's you need to! 
    // if the test code hangs, this is probably the issue.
    unimplemented();
    return 0;
}



int fd_putc(int fd, char c) {
    if(write(fd, &c, 1) != 1)
        sys_die(write, write failed);
    return 0;
}
void fd_puts(int fd, const char *msg) {
    while(*msg)
        fd_putc(fd, *msg++);
}


