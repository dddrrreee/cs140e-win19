#include "rpi.h"

// roughly 2-3x the number of cyles.  dunno if we care.  can read cycle count from
// cp15 iirc.
void delay_cycles(unsigned ticks) {
        while(ticks-- > 0)
                asm("add r1, r1, #0");
}

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
