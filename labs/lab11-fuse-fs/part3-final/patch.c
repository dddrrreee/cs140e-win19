// ignore for part1.
#include <unistd.h>
#include "redirect.h"
#include "echo-until.h"

enum { TRACE_FD_REPLAY = 11, TRACE_FD_HANDOFF };
static int is_open(int fd) {
    return fcntl(fd, F_GETFL) >= 0;
}
static int using_pi_p = 0;
static int pi_rd_fd, pi_wr_fd;
static dirent_t *console = 0;
static char prompt[] = "PIX:> ";
static int wr_console(int i) {
    char c = i;
//    cmd_append(console, "/console", &c, 1);
    fputc(c, stderr);
    return 0;
}

/*
    -d Enable debugging output (implies -f).

    -f Run in foreground; this is useful if you're running under a
    debugger. WARNING: When -f is given, Fuse's working directory is
    the directory you were in when you started it. Without -f, Fuse
    changes directories to "/". This will screw you up if you use
    relative pathnames.

    -s Run single-threaded instead of multi-threaded. 
*/
int main(int argc, char *argv[]) {
    assert(ent_lookup(root, "/console"));
    assert(ent_lookup(root, "/echo.cmd"));
    assert(ent_lookup(root, "/reboot.cmd"));
    assert(ent_lookup(root, "/run.cmd"));

    char *pi_prog = 0;
    if(argc > 3 && strcmp(argv[argc-2], "-pi") == 0) {
        assert(!argv[argc]);
        pi_prog = argv[argc-1];
        argc -= 2;
        argv[argc] = 0;
        using_pi_p = 1;
        demand(is_open(TRACE_FD_HANDOFF), no handoff?);
        note("pi prog=%s\n", pi_prog);
        if(redir(&pi_rd_fd, &pi_wr_fd, pi_prog) < 0)
            error("redir failed\n");
        // have to give the pi time to get up.
        sleep(1);
        echo_until_fn(pi_rd_fd, prompt, wr_console);
    } else
        demand(!is_open(TRACE_FD_HANDOFF), no handoff?);


    return fuse_main(argc, argv, &pi_oper, 0);
}
