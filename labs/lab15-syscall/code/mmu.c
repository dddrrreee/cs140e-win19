/*
    b4-6: before mmu enabled, the icache should be disabled and invalidated.
     icache can then be enabled at the same time as the mmu is enabled.
     clean_inv_caches();

    b4-7: strongly recommended that the code which enables or disables the 
     mmu has identical virtual and physical addresses.
*/
#include "rpi.h"
#include "mmu.h"
#include "cp15-arm.h"
#include "helper-macros.h"

static fld_t fld_mk(void) {
    fld_t f;
    // all unused fields can have 0 as default.
    memset(&f, 0, sizeof f);
    f.tag = 0b10;
    return f;
}

static void fld_check_valid(fld_t *f) {
    assert(f->_sbz1 == 0);
    assert(f->tag == 0b10);
    assert(f->S == 0);
    assert(f->IMP == 0);
    assert(f->C == 0);
    assert(f->B == 0);
    assert(f->super == 0);
}

static void fld_check(void) {
    fld_t f = fld_mk();
    fld_check_valid(&f);

    AssertNow(sizeof f == 4);

    //                    field     offset  nbits
    check_bitfield(fld_t, tag,      0,      2);
    check_bitfield(fld_t, B,        2,      1);
    check_bitfield(fld_t, C,        3,      1);
    check_bitfield(fld_t, XN,       4,      1);
    check_bitfield(fld_t, domain,   5,      4);
    check_bitfield(fld_t, IMP,      9,      1);
    check_bitfield(fld_t, AP,       10,     2);
    check_bitfield(fld_t, TEX,      12,     3);
    check_bitfield(fld_t, APX,      15,     1);
    check_bitfield(fld_t, S,        16,     1);
    check_bitfield(fld_t, nG,       17,     1);
    check_bitfield(fld_t, super,       18,     1);
    check_bitfield(fld_t, sec_base_addr, 20,     12);
}

void fld_print(fld_t *f) {
    printk("------------------------------\n");
    printk("0x%x:\n", f);
    print_field(f, sec_base_addr);
    printk("\t  --> va=0x%8x\n", f->sec_base_addr<<20);
    printk("\t           76543210\n");

    print_field(f, nG);
    print_field(f, S);
    print_field(f, APX);
    print_field(f, TEX);
    print_field(f, AP);
    print_field(f, IMP);
    print_field(f, domain);
    print_field(f, XN);
    print_field(f, C);
    print_field(f, B);
    print_field(f, tag);

    fld_check_valid(f);
}

fld_t *mmu_lookup(fld_t *pt, uint32_t va) {
    assert(is_aligned(va, 20));
    return &pt[va>>20];
}

// create a mapping for <va> to <pa> in the page table <pt>
fld_t *mmu_map_section(fld_t *pt, uint32_t va, uint32_t pa) {
    assert(is_aligned(va, 20));
    assert(is_aligned(pa, 20));

    fld_t *pte = mmu_lookup(pt, va);
    demand(!pte->tag, already set);
    *pte = fld_mk();

    pte->nG = 0;
    pte->APX = 0;
    pte->TEX = 0; 
    pte->AP = 0b11;
    pte->domain = 0;
    pte->XN = 0;
    pte->sec_base_addr = pa >> 20;

    // fld_print(pte);
    // printk("my.pte@ 0x%x = %b\n", pt, *(unsigned*)pte);
    return pte;
}

// Naive: we assume a single page table, which forces it to be 4096 entries.
// Why 4096:
//      - ARM = 32-bit addresses.
//      - 2^32 = 4GB
//      - we use 1MB sections (i.e., "page size" = 1MB).
//      - there are 4096 1MB sections in 4GB (4GB = 1MB * 4096).
//      - therefore page table must have 4096 entries.
//
// Note: If you want 4k pages, need to use the ARM 2-level page table format.
// These also map 1MB (otherwise hard to mix 1MB sections and 4k pages).
fld_t *mmu_pt_alloc(unsigned sz) {
    demand(sz = 4096, we only handling a single page table right now);

    // first-level page table is 4096 entries.
    fld_t *pt = kmalloc_aligned(4096 * 4, 1<<14);
    AssertNow(sizeof *pt == 4);
    demand(is_aligned((unsigned)pt, 14), must be 14-bit aligned!);
    return pt;
}



// i think turning caches on after off doesn't need anything special.
void mmu_all_cache_on(void) {
    cp15_ctrl_reg1_t r = cp15_ctrl_reg1_rd();
        // there are some other things too in the aux i believe: see arm1176.pdf
        mmu_dcache_on(r);
        mmu_icache_on(r);
        mmu_write_buffer_on(r); 
        mmu_predict_on(r); 
        mmu_l2cache_on(r);
    cp15_ctrl_reg1_wr(r);

    // i don't think you need to invalidate again?
    cp15_sync();
}

// turning caches off after on may need some careful thinking.
void mmu_all_cache_off(void) {
    cp15_ctrl_reg1_t r = cp15_ctrl_reg1_rd();
    mmu_dcache_off(r);
    mmu_l2cache_off(r);
    mmu_icache_off(r);
    mmu_write_buffer_off(r); 
    mmu_predict_off(r); 

    // think this has to be in assembly since the C code will do some loads and stores
    // in the window b/n disabling the dcache and cleaning an invalidating it, so 
    // there could be a dirty line in the cache that we will not handle right.
    cp15_ctrl_reg1_wr(r);
    cp15_barrier();

    cp15_dcache_clean_inv();
    cp15_icache_inv();
    cp15_sync();
}

void mmu_init(void) { 
    mmu_reset(); 

    struct control_reg1 c = cp15_ctrl_reg1_rd();
    c.XP_pt = 1;
    cp15_ctrl_reg1_wr(c);
}

// verify that interrupts are disabled.
void mmu_enable_set(cp15_ctrl_reg1_t c) {
    assert(c.MMU_enabled);
    mmu_enable_set_asm(c);
}
void mmu_disable_set(cp15_ctrl_reg1_t c) {
    assert(!c.MMU_enabled);
    assert(!c.C_unified_enable);
    mmu_disable_set_asm(c);
}
void mmu_disable(void) {
    cp15_ctrl_reg1_t c = cp15_ctrl_reg1_rd();
    assert(c.MMU_enabled);
    c.MMU_enabled=0;
    c.C_unified_enable = 0;
    mmu_disable_set_asm(c);
}
void mmu_enable(void) {
    cp15_ctrl_reg1_t c = cp15_ctrl_reg1_rd();
    assert(!c.MMU_enabled);
    c.MMU_enabled = 1;
    mmu_enable_set(c);
}
