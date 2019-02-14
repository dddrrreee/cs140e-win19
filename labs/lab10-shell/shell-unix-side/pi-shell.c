// engler: trivial shell for our pi system.  it's a good strand of yarn
// to pull to motivate the subsequent pieces we do.
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#include "pi-shell.h"
#include "demand.h"
#include "../bootloader/unix-side/support.h"

#include "../bootloader/shared-code/simple-boot.h"

// have pi send this back when it reboots (otherwise my-install exits).
static const char pi_done[] = "PI REBOOT!!!\n";
// pi sends this after a program executes to indicate it finished.
static const char cmd_done[] = "CMD-DONE\n";


/************************************************************************
 * provided support code.
 */
static void write_exact(int fd, const void *buf, int nbytes) {
        int n;
        if((n = write(fd, buf, nbytes)) < 0) {
		panic("i/o error writing to pi = <%s>.  Is pi connected?\n", 
					strerror(errno));
	}
        demand(n == nbytes, something is wrong);
}

// write characters to the pi.
static void pi_put(int fd, const char *buf) {
	int n = strlen(buf);
	demand(n, sending 0 byte string);
	write_exact(fd, buf, n);
}

// read characters from the pi until we see a newline.
int pi_readline(int fd, char *buf, unsigned sz) {
	for(int i = 0; i < sz; i++) {
		int n;
                if((n = read(fd, &buf[i], 1)) != 1) {
                	note("got %s res=%d, expected 1 byte\n", strerror(n),n);
                	note("assuming: pi connection closed.  cleaning up\n");
                        exit(0);
                }
		if(buf[i] == '\n') {
			buf[i] = 0;
			return 1;
		}
	}
	panic("too big!\n");
}


#define expect_val(fd, v) (expect_val)(fd, v, #v)
static void (expect_val)(int fd, unsigned v, const char *s) {
	unsigned got = get_uint(fd);
	if(v != got)
                panic("expected %s (%x), got: %x\n", s,v,got);
}

// print out argv contents.
static void print_args(const char *msg, char *argv[], int nargs) {
	note("%s: prog=<%s> ", msg, argv[0]);
	for(int i = 1; i < nargs; i++)
		printf("<%s> ", argv[i]);
	printf("\n");
}

// anything with a ".bin" suffix is a pi program.
static int is_pi_prog(char *prog) {
	int n = strlen(prog);

	// must be .bin + at least one character.
	if(n < 5)
		return 0;
	return strcmp(prog+n-4, ".bin") == 0;
}


/***********************************************************************
 * implement the rest.
 */

// catch control c: set done=1 when happens.  
static sig_atomic_t done = 0;
static void catch_control_c(void) {
	unimplemented();
}


// fork/exec/wait: use code from homework.
static int do_unix_cmd(char *argv[], int nargs) {
	unimplemented();
}


static void send_prog(int fd, const char *name) {
	int nbytes;

	// from homework.
        unsigned *code = (void*)read_file(&nbytes, name);
	assert(nbytes%4==0);

	expect_val(fd, ACK);
	printf("got ACK\n");

	unimplemented();
}

// ship pi program to the pi.
static int run_pi_prog(int pi_fd, char *argv[], int nargs) {
	unimplemented();
}

// run a builtin: reboot, echo, cd
static int do_builtin_cmd(int pi_fd, char *argv[], int nargs) {
	unimplemented();
}

/*
 * suggested steps:
 * 	1. just do echo.
 *	2. add reboot()
 *	3. add catching control-C, with reboot.
 *	4. run simple program: anything that ends in ".bin"
 *
 * NOTE: any command you send to pi must end in `\n` given that it reads
 * until newlines!
 */
static int shell(int pi_fd, int unix_fd) {
	const unsigned maxargs = 32;
	char *argv[maxargs];
	char buf[8192];

	catch_control_c();

	// wait for the welcome message from the pi?  note: we 
	// will hang if the pi does not send an entire line.  not 
	// sure about this: should we keep reading til newline?
	note("> ");
	while(!done && fgets(buf, sizeof buf, stdin)) {
		int n = strlen(buf)-1;
		buf[n] = 0;

		int nargs = tokenize(argv, maxargs, buf);
		// empty line: skip.
		if(!nargs)
			;
		// is it a builtin?  do it.
		else if(do_builtin_cmd(pi_fd, argv, nargs))
			;
		// if not a pi program (end in .bin) fork-exec
		else if(!run_pi_prog(pi_fd, argv, nargs))
			do_unix_cmd(argv, nargs);
		note("> ");
	}

	if(done) {
		printf("\ngot control-c: going to shutdown pi.\n");
		unimplemented();
	}
	return 0;
}

int main(void) {
	return shell(TRACE_FD_HANDOFF, 0);
}
