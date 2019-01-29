// engler
// Simple bootloader for an r/pi connected via a tty-USB device.
//
// Sketch: 
//  	1. read in program to send.
//	2. find the USB-tty serial device
//	3. open the USB-tty serial device in 8n1 mode.
//	4. send the program using a custom protocol.
//	5. echo the output from the pi.
//
// For HW1, you will write implement steps (2), (3), (4).
//
// You shouldn't have to modify any code in this file.  Though, if you find
// a bug or improve it, let me know!
//
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
	const char exit_string[] = "DONE!!!";
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

// read and echo the characters from the tty until it closes (pi rebooted)
// or we see a string indicating a clean shutdown.
static void echo(int fd, const char *portname) {
        while(1) {
                unsigned char buf [4096];
                int n = read (fd, buf, sizeof buf - 1);
		if(n>0)
			trace_read_bytes(buf, n);
                if(!n) {
                        struct stat s;
			// change this to fstat.
                        // if(stat(portname, &s) < 0) {
                        if(fstat(fd, &s) < 0) {
				fprintf(stderr, "read EOF\n");
				perror("fstat");
                                fprintf(stderr, 
					"pi connection closed.  cleaning up\n");
                                exit(0);
                        }
			// gonna have to override this if you want to test.
                        usleep(1000);
                } else if(n < 0) {
			fprintf(stderr, "ERROR: got %s res=%d\n", strerror(n),n);
                        fprintf(stderr, "pi connection closed.  cleaning up\n");
                        exit(0);
		} else {
                        buf[n] = 0;
                        // XXX printf does not flush until newline!
                        fprintf(stderr, "%s", buf);

			if(done(buf)) {
				fprintf(stderr, "\nSaw done\n");
                        	fprintf(stderr, "\nbootloader: pi exited.  cleaning up\n");
                        	exit(0);
			}
                }
	}
}

// usage: my-install [-silent] [/<dev path>]  progname
int main(int argc, char *argv[]) { 
        const char *name = "kernel.img";
	if(argc > 1) {
		argc--;
		name = argv[argc];
	}

	const char *portname = 0;
	int print_p = 1;
	for(int i = 1; i < argc; i++) {
		if(strcmp(argv[i], "-silent") == 0)
			print_p = 0;
		else if(strcmp(argv[i], "-trace") == 0)
			trace_turn_on_raw();
		else if(argv[i][0] == '/')
			portname = argv[i];
		else
                	panic("my-install: bad argument '%s'.\n", argv[i]);
	
	}
	int prog_nbytes;
        unsigned char *program = read_file(&prog_nbytes, name);

	// open tty
	int fd;
	if((fd = trace_get_fd()) < 0) {
	 	fd = open_tty(&portname);

 		// set it to be 8n1  and 115200 baud
        	fd = set_tty_to_8n1(fd, B115200, 1);

		// giving the pi side a chance to get going.
		sleep(1);
	}

	// XXX: it appears that sometimes garbage is left in the tty connection.
	// we try to drain it out.
	fprintf(stderr, "my-install: going to clean up UART\n");
	// if we don't do this, we get a weird error later where read 
	// times out.

	// wait: do we have to sleep for a bit?
	// give the pi time to boot up.
#if 0
	char ignore[1024];
	int nbytes;
	while((nbytes = read(fd, ignore, sizeof ignore)) > 0)
		fprintf(stderr, "getting %d bytes: <%s>\n",nbytes, ignore);
#endif
	fprintf(stderr, "my-install: about to boot\n");

        simple_boot(fd, program, prog_nbytes);
	if(print_p) {
		fprintf(stderr, "my-install: going to echo\n");
		echo(fd, portname);
	}
	fprintf(stderr, "my-install: Done!\n");
	return 0;
}
