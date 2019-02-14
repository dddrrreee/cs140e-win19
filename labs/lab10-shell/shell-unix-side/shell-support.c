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
#include "pi-shell.h"

/************************************************************************
 * trivial shell command processing: split a line into multiple tokens.
 */

static char *skip_space(char *p) {
        while(*p && isspace(*p))
                p++;
        return *p == 0 ? 0 : p;
}

static char *skip_nonspace(char *p) {
        int n = 0;
        while(*p && !isspace(*p))
                p++, n++;

        return n == 0 ? 0 : p;
}

// destructively splits <buf> into tokens and stores into <argv>
int tokenize(char **argv, int max_args, char *buf) {
        char *p = buf;

        for(int i = 0; i < max_args-1; i++) {
                if(!(p = skip_space(p))) {
                        argv[i] = 0;
                        return i;
                }
                argv[i] = p;

                p = skip_nonspace(p);
                assert(p);
                *p++ = 0;
        }
        panic("too many arguments for <%s>\n", buf);
}




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


// read and echo the characters from the tty until it closes (pi rebooted)
// or we see a string indicating a clean shutdown.
void echo_until(int fd, const char *msg) {
        fsm_t f = mk_fsm(msg);

        // only read until we hit the thing we are looking for.
        while(1) {
                unsigned char c;
                int n = read(fd, &c, 1);
                if(!n) {
                        struct stat s;
                        // change this to fstat.
                        if(fstat(fd, &s) < 0) {
                                note("pi connection closed.  cleaning up\n");
                                exit(0);
                        }
                        usleep(1000);
                } else if(n < 0) {
                        note("ERROR: got %s res=%d\n", strerror(n),n);
                        note("echo: pi connection closed.  cleaning up\n");
                        exit(0);
                } else {
                        fprintf(stderr, "%c", c);
                        if(fsm_chr_done(&f, c)) {
                                // note("\nSaw done\n");
                                return;
                        }
                }
        }
}


#ifdef TEST

int main(void) {
	fsm_t f = mk_fsm("DONE!!!\n");
	const char *p;
	if((p = fsm_done(&f, "adhdhaDONE!!!\ndf afd;afdjadsflkjdsf\n")))
		printf("match.  leftover=<%s>\n", p);
	else 
		printf("not match\n");

	f = mk_fsm("CMD-DONE\n");
	if((p = fsm_done(&f, "adhdhaDOCMD-DONE\nNE!!!\ndf afd;afdjadsflkjdsf\n")))
		printf("match.  leftover=<%s>\n", p);
	else 
		printf("not match\n");

	f = mk_fsm("CMD-DONE\n");
	if((p = fsm_done(&f, "aCMD-DNE\nE!!!\ndf afd;afdjadsflkjdsf\n")))
		printf("match.  leftover=<%s>\n", p);
	else 
		printf("not match\n");

	return 0;
}
#endif


