#include <assert.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "demand.h"

#define __SIMPLE_IMPL__
#include "../shared-code/simple-boot.h"

static void send_byte(int fd, unsigned char b) {
	if(write(fd, &b, 1) < 0)
		panic("write failed in send_byte\n");
}
static unsigned char get_byte(int fd) {
	unsigned char b;
	int n;
	if((n = read(fd, &b, 1)) != 1)
		panic("read failed in get_byte: expected 1 byte, got %d\n",n);
	return b;
}

// note: the other way to do is to assign these to a char array b and 
//	return *(unsigned)b
// however, the compiler doesn't have to align b to what unsigned 
// requires, so this can go awry.  easier to just do the simple way.
// we do with |= to force get_byte to get called in the right order 
// 	(get_byte(fd) | get_byte(fd) << 8 ...) 
// isn't guaranteed to be called in that order b/c | is not a seq point.
static unsigned get_uint(int fd) {
        unsigned u = get_byte(fd);
        u |= get_byte(fd) << 8;
        u |= get_byte(fd) << 16;
        u |= get_byte(fd) << 24;
        return u;
}

void put_uint(int fd, unsigned u) {
	// mask not necessary.
        send_byte(fd, (u >> 0)  & 0xff);
        send_byte(fd, (u >> 8)  & 0xff);
        send_byte(fd, (u >> 16) & 0xff);
        send_byte(fd, (u >> 24) & 0xff);
}

static void expect(const char *msg, int fd, unsigned v) {
	unsigned x = get_uint(fd);
	if(x != v) 
		panic("%s: expected %x, got %x\n", msg, v,x);
}
// we could send the packet using struct headers, etc, but 
// that's more vulnerable to mis-steps.
//
// Note: if timeout is too small you can fail here.
void simple_boot(int fd, const unsigned char * buf, unsigned n) { 
	fprintf(stderr, "sending %d bytes\n", n);

	assert(n % 4 == 0);
	// should checksum n and send that?  overkill, but seems safer.
	put_uint(fd, SOH);
	put_uint(fd, n);
	unsigned cksum = crc32(buf, n);
	put_uint(fd, cksum);

	// XXX: make get_uint() synchronous?   We set timeout to 1sec.

	// receiver echos back --- this makes it easier to detect a bunch
	// of simple setup errors.
	assert(get_uint(fd) == SOH);
	unsigned n_cksum = crc32(&n, sizeof n);
	int res = get_uint(fd);
	if(res != n_cksum)
		panic("expected %x for crc32(nbytes), got back %x\n", 
			n_cksum, res);
	expect("checksum error", fd, cksum);
	put_uint(fd, ACK);
#if 1
	// slow it down.
        for(int i = 0; i < n; i++)
                send_byte(fd, buf[i]);
#else
	if(write(fd, buf, n) != n)
		panic("write failed in send_byte\n");
#endif
	put_uint(fd, EOT);

	unsigned rx = get_uint(fd);
	if(rx != ACK)
		panic("expected ack, got: %x\n", rx);
	// put_uint(fd, ACK);
	fprintf(stderr, "sent True\n");
}
