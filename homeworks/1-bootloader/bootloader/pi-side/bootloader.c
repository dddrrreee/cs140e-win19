/* 
 * very simple bootloader.  more robust than xmodem.   (that code seems to 
 * have bugs in terms of recovery with inopportune timeouts.)
 */

#define __SIMPLE_IMPL__
#include "../shared-code/simple-boot.h"

#include "libpi.small/rpi.h"

static void send_byte(unsigned char uc) {
	uart_putc(uc);
}
static unsigned char get_byte(void) { 
        return uart_getc();
}

static unsigned get_uint(void) {
	unsigned u = get_byte();
        u |= get_byte() << 8;
        u |= get_byte() << 16;
        u |= get_byte() << 24;
	return u;
}
static void put_uint(unsigned u) {
        send_byte((u >> 0)  & 0xff);
        send_byte((u >> 8)  & 0xff);
        send_byte((u >> 16) & 0xff);
        send_byte((u >> 24) & 0xff);
}

static void die(int code) {
        put_uint(code);
        reboot();
}

//  bootloader:
//	1. wait for SOH, size, cksum from unix side.
//	2. echo SOH, checksum(size), cksum back.
// 	3. wait for ACK.
//	4. read the bytes, one at a time, copy them to ARMBASE.
//	5. verify checksum.
//	6. send ACK back.
//	7. wait 500ms 
//	8. jump to ARMBASE.
//
void notmain(void) {
	uart_init();

	// XXX: cs107e has this delay; doesn't seem to be required if 
	// you drain the uart.
	delay_ms(500);


	/* XXX put your bootloader implementation here XXX */


	// XXX: appears we need these delays or the unix side gets confused.
	// I believe it's b/c the code we call re-initializes the uart; could
	// disable that to make it a bit more clean.
	delay_ms(500);

	// run what client sent.
        BRANCHTO(ARMBASE);
	// should not get back here, but just in case.
	reboot();
}
