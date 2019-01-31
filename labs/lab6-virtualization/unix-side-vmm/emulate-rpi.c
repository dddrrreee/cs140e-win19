// currently delay_ms will just work.
// however, it's not going to play nice with multiple processes.


#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <unistd.h>

#include "demand.h" 
#include "pi-vmm-ops.h"
#include "vmm.h"

static int pi_fd = 3;

// using standard headers is tricky here since we redefined some symbols.
#define FAKE_PI_IMPL
#include "rpi.h"

static void send_byte(unsigned char b) {
        if(write(pi_fd, &b, 1) < 0)
                panic("write failed in send_byte\n");
}
static unsigned char get_byte(void) {
        unsigned char b;
        int n;
        if((n = read(pi_fd, &b, 1)) != 1)
                panic("read failed in get_byte: expected 1 byte, got %d\n",n);
        return b;
}

// NOTE: the other way to do is to assign these to a char array b and 
//      return *(unsigned)b
// however, the compiler doesn't have to align b to what unsigned 
// requires, so this can go awry.  easier to just do the simple way.
// we do with |= to force get_byte to get called in the right order 
//      (get_byte(fd) | get_byte(fd) << 8 ...) 
// isn't guaranteed to be called in that order b/c | is not a seq point.
unsigned get_uint(void) {
        unsigned u = get_byte();
        u |= get_byte() << 8;
        u |= get_byte() << 16;
        u |= get_byte() << 24;
        return u;
}

void put_uint(unsigned u) {
        // mask not necessary.
        send_byte((u >> 0)  & 0xff);
        send_byte((u >> 8)  & 0xff);
        send_byte((u >> 16) & 0xff);
        send_byte((u >> 24) & 0xff);
}


void rpi_reboot(void) {
	printf("in reboot\n");
	unimplemented();
	printf("done\n");
	exit(0);
}
void rpi_clean_reboot(void) {
	printf("clean reboot\n");
	rpi_reboot();
}

void rpi_PUT32(unsigned addr, unsigned v) {
//	printf("need to send to the pi PUT32(<%x,%x>)\n", addr, v);
	unimplemented();
}
void rpi_put32(volatile void *addr, unsigned v) {
	rpi_PUT32((unsigned long)addr, v);
}

unsigned rpi_GET32(unsigned addr) {
//	printf("need to send to the pi GET32(<%x>)\n", addr);
	unimplemented();
}
unsigned rpi_get32(const volatile void *addr) {
	return rpi_GET32((unsigned long)addr);
}

/*********************************************************************
 * can provide a fake implementation that just calls the system 
 * implementation.  or can actually pull in uart and make a smarter
 * driver on the pi that uses the addresses to decide what to do.
 *
 * this is a common tension in vmms --- the OSes will do alot of 
 * device manipulations by reading/writing different addresses
 * and you have to figure out what they are doing and what you 
 * should do in response.
 */
void rpi_uart_init(void) {}

int rpi_uart_getc(void);

void rpi_uart_putc(unsigned c) {
	unimplemented();
}
