/*
   build a simple system call framework: look at int_part0 and int_part1.
*/
#include "rpi.h"
#include "cp15-arm.h"
#include "mmu.h"
#include "interrupts-asm.h"
#include "rpi-interrupts.h"

/****************************************************************************************
 * helper code to help set up address space environment.
 */
#include "bvec.h"

typedef struct env {
    uint32_t pid,
             domain,
             asid;

    // the domain register.
    uint32_t domain_reg;
    fld_t *pt;
} env_t;

static bvec_t dom_v, asid_v, env_v;
static uint32_t pid_cnt;

#define MAX_ENV 8
static env_t envs[MAX_ENV];

void env_init(void) {
    dom_v = bvec_mk(1,16);
    asid_v = bvec_mk(1,64);
    env_v = bvec_mk(0,MAX_ENV);
}

env_t *env_alloc(void) {
    env_t *e = &envs[bvec_alloc(&env_v)];

    e->pt = mmu_pt_alloc(4096);
    e->pid = ++pid_cnt;
    e->domain = bvec_alloc(&dom_v);
    e->asid = bvec_alloc(&asid_v);

    // default: can override.
    e->domain_reg = 0b01 << e->domain*2;

    return e;
}
void env_free(env_t *e) {
    unsigned n = e - &envs[0];
    demand(n < MAX_ENV, freeing unallocated pointer!);

    bvec_free(&dom_v, e->domain);
    bvec_free(&asid_v, e->asid);
    bvec_free(&env_v, n);

    // not sure how to free pt.  ugh.
}

void env_switch_to(env_t *e) {
    cp15_domain_ctrl_wr(e->domain_reg);
    cp15_domain_ctrl_wr(~0UL);

    cp15_set_procid_ttbr0(e->pid << 8 | e->asid, e->pt);

    unsigned pid,asid;
    mmu_get_curpid(&pid, &asid);
    printk("pid=%d, expect=%d\n", pid, e->pid);
    printk("asid=%d, expect=%d\n", asid, e->asid);

    assert(pid == e->pid);
    assert(asid == e->asid);
    // mmu_asid_print();

    mmu_enable();
}


/*************************************************************************************
 * your code
 */

unsigned cpsr_read(void);

unsigned cpsr_read_c(void) { 
    return cpsr_read() & 0b11111; 
}
int mmu_is_on(void) {
    return cp15_ctrl_reg1_rd().MMU_enabled;
}
void cpsr_print_mode(unsigned cpsr_r) {
    switch(cpsr_r & 0b11111) {
    case USER_MODE: printk("user mode\n"); break;
    case FIQ_MODE: printk("fiq mode\n"); break;
    case IRQ_MODE: printk("irq mode\n"); break;
    case SUPER_MODE: printk("supervisor mode\n"); break;
    case ABORT_MODE: printk("abort mode\n"); break;
    case UNDEF_MODE: printk("undef mode\n"); break;
    case SYS_MODE: printk("sys mode\n"); break;
    default: panic("invalid cpsr: %b\n", cpsr_r);
    }
}

// you will call this with the pc of the SWI instruction, and the saved registers
// in saved_regs.  r0 at offset 0, r1 at offset 1, etc.
void handle_swi(uint8_t sysno, uint32_t pc, uint32_t *saved_regs) {
    printk("sysno=%d\n", sysno);
    printk("\tcpsr =%x\n", cpsr_read());
    assert(cpsr_read_c() == SUPER_MODE);

    // check that the stack is in-bounds.
    int i;
    assert(&i < (int*)SWI_STACK_ADDR);
    assert(&i > (int*)SYS_STACK_ADDR);

    printk("\treturn=%x stack=%x\n", pc, saved_regs);
    printk("arg[0]=%d, arg[1]=%d, arg[2]=%d, arg[3]=%d\n", 
                saved_regs[0], 
                saved_regs[1], 
                saved_regs[2], 
                saved_regs[3]);

    saved_regs[0] = 13;
    return;
}

/****************************************************************************************
 * part0, implement:
 *  - unsigned cpsr_read(void);
 *  - swi_setup_stack: set the stack pointer in SUPER mode to SWI_STACK_ADDR
 *  - swi_asm:  issue a SWI exception.
 *  - exception handling: write an SWI exception handler based on lab7-interrupts/timer-int
 */

// you need to define these.
unsigned cpsr_read(void);
int swi_asm1(int arg0, int arg1, int arg2, int arg3);
void swi_setup_stack(unsigned stack_addr);

// don't modify this: it should run fine when everything works.
void int_part0(void) {
    int_init();
    swi_setup_stack(SWI_STACK_ADDR);

    printk("about to do a SWI\n");
    int res = swi_asm1(1,2,3,4);
    printk("done with SWI, result = %d\n", res);
}

/*******************************************************************************
 * part1
 */

/*
 * for this: run with virtual memory.  actually should work out of the box.
 * then start tuning.  
 *  - write a tight loop that calls a no-op syscall.
 *  - start making it very fast.
 * you shouldn't have to touch much of this code.
 */
void int_part1(void) {
    int_init();
    env_init();
    mmu_init();
    assert(cpsr_read_c() == SYS_MODE);

    env_t *e = env_alloc();

    // map the sections you need.
    unimplemented();

    // gpio
    mmu_map_section(e->pt, 0x20000000, 0x20000000)->domain = e->domain;
    mmu_map_section(e->pt, 0x20200000, 0x20200000)->domain = e->domain;

    env_switch_to(e);
    assert(cpsr_read_c() == SYS_MODE);
    assert(mmu_is_on());
    
    swi_setup_stack(SWI_STACK_ADDR);

    uint32_t cpsr_before = cpsr_read_c();
    uint32_t reg1_before = cp15_ctrl_reg1_rd_u32();
    printk("about to call swi\n");
    printk("swi_asm = %d\n", swi_asm1(1,2,3,4));
    assert(cpsr_before == cpsr_read_c());
    assert(reg1_before == cp15_ctrl_reg1_rd_u32());

    // have to disable mmu before reboot.  probably should build in.
    mmu_disable();
    clean_reboot();
}

/**********************************************************************
 * implement part0, part1, part2.
 */
void notmain() {
    uart_init();

    // start the heap after the max stack address
    kmalloc_set_start(MAX_STACK_ADDR);

    // implement swi interrupts without vm
    int_part0();
#if 0
    // implement swi interrupts with vm
    int_part1();
#endif
    clean_reboot();
}
