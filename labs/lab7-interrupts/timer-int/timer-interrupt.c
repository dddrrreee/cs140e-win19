/*
	simple timer interrupt demonstration.

      Good timer / interrupt discussion at:
 	http://www.valvers.com/open-software/raspberry-pi/step04-bare-metal-programming-in-c-pt4/

	Most of this code is from here.
*/
#include "rpi.h"
#include "timer-interrupt.h"

volatile rpi_irq_controller_t *rpiIRQController = (void*)0x2000B200;
volatile rpi_irq_controller_t* RPI_GetIRQController(void) {
        dmb();
        dsb();
	return rpiIRQController;
}
// only the lower 8 bits seem to be used --- higher ones are GPU enabled.
unsigned rpi_extract_basic_int(unsigned x) { 
	return x & 0xff; 
}

/** @brief See the documentation for the ARM side timer (Section 14 of the
    BCM2835 Peripherals PDF) */
volatile rpi_arm_timer_t *rpiArmTimer = (void*)0x2000B400;
volatile rpi_arm_timer_t* RPI_GetArmTimer(void) {
        dmb();
        dsb();
	return rpiArmTimer;
}

void timer_interrupt_init(unsigned ncycles) {
//	install_int_handlers();

	// BCM2835 manual, section 7.5
  	PUT32(INTERRUPT_DISABLE_1, 0xffffffff);
  	PUT32(INTERRUPT_DISABLE_2, 0xffffffff);
        dmb();
        dsb();

  	// Bit 52 in IRQ registers enables/disables all GPIO interrupts
#if 0
  	// Bit 52 is in the second register, so subtract 32 for index
  	PUT32(INTERRUPT_ENABLE_2, (1 << (52 - 32)));
        dmb();
        dsb();
#endif

	// from valvers:
        //	 Enable the timer interrupt IRQ
        RPI_GetIRQController()->Enable_Basic_IRQs = RPI_BASIC_ARM_TIMER_IRQ;

        /* Setup the system timer interrupt */
        /* Timer frequency = Clk/256 * 0x400 */
        // RPI_GetArmTimer()->Load = 0x400;
        // RPI_GetArmTimer()->Load = 0x100;
        // RPI_GetArmTimer()->Load = 0x4;
        RPI_GetArmTimer()->Load = ncycles;

        // Setup the ARM Timer 
        RPI_GetArmTimer()->Control =
                RPI_ARMTIMER_CTRL_23BIT |
                RPI_ARMTIMER_CTRL_ENABLE |
                RPI_ARMTIMER_CTRL_INT_ENABLE |
                RPI_ARMTIMER_CTRL_PRESCALE_256;

  	// Now GPIO interrupts are enabled, but interrupts are still
  	// globally disabled. Caller has to enable them when they are 
	// ready.
}
