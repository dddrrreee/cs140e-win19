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
#include "../shared-code/simple-boot.h"

// XXX: if anyone figures out a cleaner way to do this, lmk.   I don't
// have a mac, so stopped as soon as we had something that worked on 
// linux & macos.
// params:
//	- <timeout> is in seconds (< 1 ok)
// 	- <speed> is baud rate.
int set_tty_to_8n1(int fd, unsigned speed, double timeout) {
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
                panic("tcgetattr failed\n");
        memset (&tty, 0, sizeof tty);

	// https://github.com/rlcheng/raspberry_pi_workshop
	cfsetspeed(&tty, speed);

        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars

	// XXX: wait, does this disable break or ignore-ignore break??
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
	assert(timeout < 100 && timeout > 0);
	// 0.1 seconds read timeout
        tty.c_cc[VTIME] = (int)timeout *10;            

	/*
	 * Setup TTY for 8n1 mode, used by the pi UART.
	 */

	// Disables the Parity Enable bit(PARENB),So No Parity 
	tty.c_cflag &= ~PARENB; 	
	// CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit 
	tty.c_cflag &= ~CSTOPB;   	
	// Clears the mask for setting the data size     
	tty.c_cflag &= ~CSIZE;	 	
	// Set the data bits = 8
	tty.c_cflag |=  CS8; 		
	// No Hardware flow Control 
	tty.c_cflag &= ~CRTSCTS;
	// Enable receiver,Ignore Modem Control lines 
	tty.c_cflag |= CREAD | CLOCAL; 	
		
	// Disable XON/XOFF flow control both i/p and o/p
	tty.c_iflag &= ~(IXON | IXOFF | IXANY);    
	// Non Cannonical mode 
	tty.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  
	// No Output Processing
	tty.c_oflag &= ~OPOST;	

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                panic("tcsetattr failed\n");
	return fd;
}

// simple state machine to indicate when we've seen a special string
// from the pi telling us to shutdown.
static int done(char *s) {
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

// read and echo the characters from the tty until it closes (pi rebooted)
// or we see a string indicating a clean shutdown.
static void echo(int fd, const char *portname) {
        while(1) {
                char buf [4096];
                int n = read (fd, buf, sizeof buf - 1);
                if(!n) {
                        struct stat s;
                        if(stat(portname, &s) < 0) {
                                fprintf(stderr, 
					"pi connection closed.  cleaning up\n");
                                exit(0);
                        }
                        usleep(1000);
                } else if(n < 0) {
                        fprintf(stderr, "pi connection closed.  cleaning up\n");
                        exit(0);
		} else {
                        buf[n] = 0;
                        // XXX printf does not flush until newline!
                        fprintf(stderr, "%s", buf);

			if(done(buf)) {
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
	if(argc > 3)
                panic("my-install: too many arguments: argc=%d\n", argc);

	const char *portname = 0;
	int print_p = 1;
	for(int i = 1; i < argc; i++) {
		if(strcmp(argv[i], "-silent") == 0)
			print_p = 0;
		else if(argv[i][0] == '/')
			portname = argv[i];
		else
                	panic("my-install: bad argument '%s'.\n", argv[i]);
	
	}
	int prog_nbytes;
        unsigned char *program = read_file(&prog_nbytes, name);

	// open tty
	int fd = open_tty(&portname);
 	// set it to be 8n1  and 115200 baud
        fd = set_tty_to_8n1(fd, B115200, 2);

	// XXX: it appears that sometimes garbage is left in the tty connection.
	// we try to drain it out.
	fprintf(stderr, "my-install: going to clean up UART\n");
	char ignore[1024];
	int nbytes;
	while((nbytes = read(fd, ignore, sizeof ignore)) > 0)
		fprintf(stderr, "getting %d bytes\n",nbytes);
	fprintf(stderr, "my-install: about to boot\n");

        simple_boot(fd, program, prog_nbytes);
	if(print_p) 
		echo(fd, portname);
	fprintf(stderr, "my-install: Done!\n");
	return 0;
}
