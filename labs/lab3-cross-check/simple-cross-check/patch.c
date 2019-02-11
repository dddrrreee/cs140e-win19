// delete these two functions in simple-check.c and put these there instead.
// at the end of simple-check.c

static int pin_gen(unsigned *out) {
        static unsigned u = 0;
        if(u > 70) {
                u = 0;
                return 0;
        }
        *out = (u < 64) ? u : random();
        u++;
        return 1;
}

gen_t xc_get_pin_gen(void) {
        return pin_gen;
}
