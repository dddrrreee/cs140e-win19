#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "demand.h"
#include "echo-until.h"

/**********************************************************************
 * trivial FSM for matching on strings for the pi.
 */

typedef struct {
        int pos;
        int fsm_end;
        char exit_string[1024];
} fsm_t;

fsm_t mk_fsm(const char *str) {
        fsm_t f;
        int n = strlen(str);

        demand(n < sizeof f.exit_string-1, string too big);

        f.pos = 0; 
        f.fsm_end = n;
        strcpy(f.exit_string, str);

        return f; 
}

int fsm_chr_done(fsm_t *f, unsigned char c) {
	assert(f->pos < f->fsm_end);
	if(c != f->exit_string[f->pos++])
        	f->pos = 0;
	else if(f->pos == f->fsm_end)
        	return 1;
	return 0;
}

unsigned char *fsm_done(fsm_t *f, unsigned char *s) {
        for(; *s; s++)
		if(fsm_chr_done(f, *s))
			return s;
        return 0;
}

int echo_until_fn(int fd, const char *msg, int (*fn)(int c)) {
        fsm_t f = mk_fsm(msg);

        // only read until we hit the thing we are looking for.
        while(1) {
                unsigned char c;
                int n = read(fd, &c, 1);
                if(!n) {
                    fprintf(stderr, "echo_until:pi connection closed. done.\n");
                    return 0;
                } else if(n < 0 || fn(c) < 0) {
                        note("ERROR: got %s res=%d\n", strerror(n),n);
                        note("echo: pi connection closed.  cleaning up\n");
                        return 0;
                        exit(0);
                } else {
                        if(fsm_chr_done(&f, c))
                                return 1;
                }
        }
    return 1;
}

static int echo_c(int c) {
    fprintf(stderr, "%c", c);
    return 0;
}

// read and echo the characters from the tty until it closes (pi rebooted)
// or we see a string indicating a clean shutdown.
int echo_until(int fd, const char *msg) {
    return echo_until_fn(fd, msg, echo_c);
}
