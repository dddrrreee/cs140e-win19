#include "rpi.h"
#include "pi-vmm-ops.h"

// note: printf will not work unless you do something special.
#undef printf
#define printf  cannot_print

// just from your bootloader.
static void send_byte(unsigned char uc) {
        uart_putc(uc);
}
static unsigned char get_byte(void) {
        return uart_getc();
}

unsigned get_uint(void) {
        unsigned u = get_byte();
        u |= get_byte() << 8;
        u |= get_byte() << 16;
        u |= get_byte() << 24;
        return u;
}

void put_uint(unsigned u) {
        send_byte((u >> 0)  & 0xff);
        send_byte((u >> 8)  & 0xff);
        send_byte((u >> 16) & 0xff);
        send_byte((u >> 24) & 0xff);
}

// probably should put reply's and CRC32
int notmain ( void ) {
	uart_init();
	delay_ms(100);

        put_uint(OP_READY);
        unsigned addr;
        while(1) {
		/* have loop to handle requests, including OP_REBOOT */
	}
}
