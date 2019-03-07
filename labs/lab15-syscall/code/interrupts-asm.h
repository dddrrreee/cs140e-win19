#ifndef __INTERRUPT_ASM_H__
#define __INTERRUPT_ASM_H__

/* from A2-2 */
#define USER_MODE       0b10000
#define FIQ_MODE        0b10001
#define IRQ_MODE        0b10010
#define SUPER_MODE      0b10011
#define ABORT_MODE      0b10111
#define UNDEF_MODE      0b11011
#define SYS_MODE         0b11111

#define SYS_STACK_ADDR 0x100000
#define SWI_STACK_ADDR 0x200000
// general interrupts.
#define INT_STACK_ADDR 0x300000

#define MAX_ADDR       0x400000

#define MAX_STACK_ADDR  INT_STACK_ADDR

#endif
