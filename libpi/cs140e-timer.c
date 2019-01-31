#include "rpi.h"

#if 0
void delay(unsigned ticks) {
        while(ticks-- > 0)
                asm("add r1, r1, #0");
}
#endif

unsigned timer_get_time(void) {
	return GET32(0x20003004);
}

void delay_us(unsigned us) {
    unsigned rb = timer_get_time();
    while (1) {
        unsigned ra = timer_get_time();
        if ((ra - rb) >= us) {
            break;
        }
    }
}

void delay_ms(unsigned ms) {
	delay_us(ms*1000);
}
