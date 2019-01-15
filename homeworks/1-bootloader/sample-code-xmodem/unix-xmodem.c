// engler: this is a unix side program to send bytes to the pi bootloader using 
// the xmodem protocol.
//
// the high bit: it's very complicated.
// Homework 1 = you will build a much simpler one.

#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "demand.h"

static unsigned min(unsigned x, unsigned y) { return x < y ? x : y; }

enum {
        SOH = 0x01,   // Start Of Header
        ACK = 0x06,   // Acknowledge (positive)
        NAK = 0x15,   // Acknowledge (negative)
        EOT = 0x04,   // End of transmission 
        PAYLOAD_SZ = 128,
};

static void drain(int fd) {
	unsigned char b;
	while(read(fd, &b, 1) == 1)
		;
}

static void send_byte(int fd, unsigned char b) {
	if(write(fd, &b, 1) < 0)
		panic("write failed in send_byte\n");
}

static unsigned char get_byte(int fd) {
	unsigned char b;
	int n;
	if((n = read(fd, &b, 1)) != 1)
		panic("read failed in send_byte: expected 1 byte, got %d\n",n);
	return b;
}

/*
 * tricky part: there is a race condition in that the pi can have a queue
 * of naks sent to us and we will not know necessarily which one we have.
 * so we keep sending the first block until it sends us an ack.
 */
static int send_block(int fd, unsigned blk, const unsigned char *b, int n) {
	unsigned char payload[PAYLOAD_SZ];
	
	// hack so we don't read beyond the buffer at the end.
	memcpy(payload, b, n);

	// keep retransmitting this block until no NAKs
	while(1) {
		//printf("sending block %d\n", blk);
		send_byte(fd, SOH);
		send_byte(fd, blk);
		send_byte(fd, 0xff - blk);

		unsigned char cksum;
		int i;
		for(cksum = i = 0; i < PAYLOAD_SZ; i++) {
			cksum += payload[i];
			send_byte(fd, payload[i]);
		}
		send_byte(fd, cksum);
		
		unsigned char rx = get_byte(fd);
		// received it
		if(rx == ACK)
			return 1;
		else if(rx != NAK)
			panic("Invalid reply from pi: <%x>\n", rx);
		else {
			// in case we got a bunch of naks, drain them.
			drain(fd);
			continue;
		}
	}
}

void xmodem(int fd, const unsigned char * buf, unsigned n) { 
	unsigned nblocks = floor((n+PAYLOAD_SZ-1.0) / (double)PAYLOAD_SZ);
	printf("sending %u bytes (%d blocks) to bootloader\n", n, nblocks);
	drain(fd);

	int block = 1;
	const unsigned char *p, *e;
	for(p = buf, e = p + n; p < e; p += 128, block++)
		if(!send_block(fd, block, p,  min(128, e - p))) {
			fprintf(stderr, "xmodem: send failed!!\n");
			return;
		}

	send_byte(fd, EOT);
	unsigned rx = get_byte(fd);
	if(rx != ACK)
		panic("expected ack, got: %c\n", rx);
		
	printf("sent True\n");
}
