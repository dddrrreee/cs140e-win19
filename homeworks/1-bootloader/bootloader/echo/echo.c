// engler
// simple echo for used with bootloader.

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>

#include "demand.h"
#include "support.h"
#include "trace.h"
#include "../shared-code/simple-boot.h"
#include "tty.h"

// simple state machine to indicate when we've seen a special string
// from the pi telling us to shutdown.
static int done(unsigned char *s) {
	static unsigned pos = 0;
	const char exit_string[] = "DONE!!!\n";
	const int n = sizeof exit_string - 1;

	for(; *s; s++) {
		assert(pos < n);
		if(*s != exit_string[pos++]) {
			pos = 0;
			return done(s+1); // check remainder
		}
		// maybe should check if "DONE!!!" is last thing printed?
		if(pos == sizeof exit_string - 1)
			return 1;
	}
	return 0;
}

#include <errno.h>

#define note(msg...) do {                                              \
        fprintf(stderr, "echo:"); 					\
        fprintf(stderr, ##msg);                                         \
} while(0)


// read and echo the characters from the tty until it closes (pi rebooted)
// or we see a string indicating a clean shutdown.
static void echo(int fd) {
        while(1) {
                unsigned char buf [4096];
                int n = read (fd, buf, sizeof buf - 1);
                if(!n) {
                        struct stat s;
			// change this to fstat.
                        if(fstat(fd, &s) < 0) {
				note("read EOF\n");
				perror("fstat");
                                note("pi connection closed.  cleaning up\n");
                                exit(0);
                        }
			// gonna have to override this if you want to test.
                        usleep(1000);
                } else if(n < 0) {
			note("ERROR: got %s res=%d\n", strerror(n),n);
                        note("echo: pi connection closed.  cleaning up\n");
                        exit(0);
		} else {
                        buf[n] = 0;
                        // XXX printf does not flush until newline!
                        fprintf(stderr, "%s", buf);

			if(done(buf)) {
				note("\necho: Saw done\n");
                        	note("\necho: bootloader: pi exited.  cleaning up\n");
                        	exit(0);
			}
                }
	}
}
int main(void) {
	echo(TRACE_FD_HANDOFF);
}
