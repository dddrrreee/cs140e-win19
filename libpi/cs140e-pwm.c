// To play around with it on your laptop:
//	gcc -DUNIX cs140e-pwm.c

#ifdef UNIX
#	include <stdio.h>
#	include <assert.h>
#	define printk printf
#else
#	include "rpi.h"
#	include "pwm.h"
#endif

/*
 * compute pwm using a integer line drawing algorithm.   these integer
 * line drawings come up everywhere: propertional share CPU scheduling,
 * noise minimization, ...
 */
// pwm = a line drawing algorithm.
// https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm
static void int_line(unsigned *pwm_choice, unsigned n, int x1, int y1) {
        int pos = 0,
                x0 = 0,
                y0 = 0,
                dx = x1,
                dy = y1,
                err = (dx>dy ? dx : -dy)/2, e2;

        assert(n==x1+y1);

        for(;;){
                if (x0==x1 && y0==y1) break;
                e2 = err;
                if (e2 >-dx) { err -= dy; x0++; pwm_choice[pos++] = 1; }
                if (e2 < dy) { err += dx; y0++; pwm_choice[pos++] = 0; }
        }
        assert(pos == n);
}

void pwm_compute(unsigned *pwm_choices, unsigned on, unsigned n) {
	assert(on<=n);
	unsigned off = n-on;
	int_line(pwm_choices,n,on,off);
}

// if you want to see what the on/off decisions are.
void pwm_print(unsigned *pwm_choices, unsigned on, unsigned n) {
	assert(on<=n);
	unsigned off = n-on;
	printk("------------------------------------------------\n");
	printk("n=%d, on=%d, off=%d\n", on+off,on,off);
	for(int i = 0; i < off+on; i++)
		printk("\tpwm[%d]=%d\n", i,pwm_choices[i]);
}


#ifdef UNIX
int main(void) {
        const unsigned N = 10;
        unsigned u[N];
        pwm_compute(u, 3, N);
        pwm_print(u, 3, N);
	return 0;
}
#endif
