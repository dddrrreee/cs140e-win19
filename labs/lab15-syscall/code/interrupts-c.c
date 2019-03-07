/* 
 * interrupts-c.c: some interrupt support code.  Default handlers, 
 * interrupt installation.
 */
#include "rpi.h"
#include "rpi-interrupts.h"

#define UNHANDLED(msg,r) \
	panic("ERROR: unhandled exception <%s> at PC=%x\n", msg,r)

void interrupt_vector(unsigned pc) {
	UNHANDLED("general interrupt", pc);
}
void fast_interrupt_vector(unsigned pc) {
	UNHANDLED("fast", pc);
}
void software_interrupt_vector(unsigned pc) {
	UNHANDLED("soft interrupt", pc);
}
void reset_vector(unsigned pc) {
	UNHANDLED("reset vector", pc);
}
void undefined_instruction_vector(unsigned pc) {
	UNHANDLED("undefined instruction", pc);
}
void prefetch_abort_vector(unsigned pc) {
	UNHANDLED("prefetch abort", pc);
}
void data_abort_vector(unsigned pc) {
	UNHANDLED("data abort", pc);
}

static int int_intialized_p = 0;

// wait: which was +8
enum {
    RESET_INC = -1,     // cannot return from reset
    UNDEFINED_INC = 4,  // 
    SWI_INC = 4,        // address of instruction after SWI
    PREFETCH_INC = 4,        // aborted instruction + 4
};

// call before int_init() to override.
void int_set_handler(int t, interrupt_t handler) {
    interrupt_t *src = (void*)&_interrupt_table;
    demand(t >= RESET_INT && t < FIQ_INT && t != INVALID, invalid type);
    src[t] = handler;

    demand(!int_intialized_p, must be called before copying vectors);
}

/*
 * Copy in interrupt vector table and FIQ handler _table and _table_end
 * are symbols defined in the interrupt assembly file, at the beginning
 * and end of the table and its embedded constants.
 */
static void install_handlers(void) {
        unsigned *dst = (void*)RPI_VECTOR_START,
                 *src = &_interrupt_table,
                 n = &_interrupt_table_end - src;
        for(int i = 0; i < n; i++)
                dst[i] = src[i];
}

#include "cp15-arm.h"

void int_init(void) {
    // BCM2835 manual, section 7.5: turn off all GPIO interrupts.
    PUT32(INTERRUPT_DISABLE_1, 0xffffffff);
    PUT32(INTERRUPT_DISABLE_2, 0xffffffff);
    system_disable_interrupts();
    cp15_barrier();

    // setup the interrupt vectors.
    install_handlers();
    int_intialized_p = 1;
}
