#include "rpi.h"

int gpio_broken_example(unsigned pin) {
        if(pin >= 32)
                return -1;
        volatile unsigned char *u = (void*)(0x80000000);
        u += pin*4;
        put32(u, get32(u)+1);
        return 0;
}

void notmain(void) {
	gpio_broken_example(4);
}
