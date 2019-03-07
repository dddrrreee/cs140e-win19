/*
 * low-level cp15 helper routines.  
 */
#include "rpi.h"
#include "mmu.h"
#include "cp15-arm.h"
#include "helper-macros.h"

/*************************************************************************
 * control reg 1: b4-40
 */

// actually: not sure.
struct control_reg1 control_reg1_mk(void) {
    struct control_reg1 r;
    memset(&r, 0, sizeof r);
    return r;
}

void check_control_reg1(void) {
    AssertNow(sizeof(struct control_reg1) == 4);
    check_bitfield(struct control_reg1, MMU_enabled,        0,      1);
    check_bitfield(struct control_reg1, A_alignment,        1,      1);
    check_bitfield(struct control_reg1, C_unified_enable,   2,      1);
    check_bitfield(struct control_reg1, W_write_buf,        3,      1);
    check_bitfield(struct control_reg1, B_endian,           7,      1);
    check_bitfield(struct control_reg1, S_prot,             8,      1);
    check_bitfield(struct control_reg1, R_rom_prot,         9,      1);
    check_bitfield(struct control_reg1, F,                  10,      1);
    check_bitfield(struct control_reg1, Z_branch_pred,      11,      1);
    check_bitfield(struct control_reg1, I_icache_enable,    12,      1);
    check_bitfield(struct control_reg1, V_high_except_v,    13,      1);
    check_bitfield(struct control_reg1, RR_cache_rep,       14,      1);
    check_bitfield(struct control_reg1, L4,                 15,      1);
    check_bitfield(struct control_reg1, F1,                 21,      1);
    check_bitfield(struct control_reg1, U_unaligned,        22,      1);
    check_bitfield(struct control_reg1, XP_pt,              23,      1);
    check_bitfield(struct control_reg1, VE_vect_int,        24,      1);
    check_bitfield(struct control_reg1, EE,                 25,      1);
    check_bitfield(struct control_reg1, L2_enabled,         26,      1);
    check_bitfield(struct control_reg1, TR_tex_remap,       28,      1);
    check_bitfield(struct control_reg1, FA_force_ap,        29,      1);
}

void control_reg1_valid(struct control_reg1 *r) {
    // SBO: should be 1
    assert(r->_unused1 == 0b111);
    assert(r->_dt == 1);
    assert(r->_it == 1);

    // SBZ: should be 0
    assert(r->_sbz0 == 0);
    assert(r->_sbz1 == 0);
    assert(r->_st == 0);

    // unknown
    // assert(r->_reserved == 0);

    // allow non-back compat.  
    // assert(r->XP_pt == 1);

    // we don't enable cache yet.
    assert(!r->L2_enabled);
    assert(!r->I_icache_enable);
    assert(!r->C_unified_enable);
}
    
void control_reg1_print(struct control_reg1 *r) {
    control_reg1_valid(r);

    printk("system control reg=\n");
    print_field(r, FA_force_ap);
    print_field(r, TR_tex_remap);
    print_field(r, L2_enabled);
    print_field(r, EE);
    print_field(r, VE_vect_int);
    print_field(r, XP_pt);
    print_field(r, U_unaligned);
    print_field(r, F1);
    print_field(r, RR_cache_rep);
    print_field(r, I_icache_enable);
    print_field(r, Z_branch_pred);
    print_field(r, R_rom_prot);
    print_field(r, S_prot);
    print_field(r, B_endian);
    print_field(r, W_write_buf);
    print_field(r, C_unified_enable);
    print_field(r, A_alignment);
    print_field(r, MMU_enabled);
}

/*************************************************************************
 * tlb config: b4-39
 */
static void check_tlb_config(void) { 
    AssertNow(sizeof(cp15_tlb_config_t) == 4);
    check_bitfield(cp15_tlb_config_t, unified_p,    0,      1);
    check_bitfield(cp15_tlb_config_t, n_d_lock,     8,      8);
    check_bitfield(cp15_tlb_config_t, n_i_lock,    16,      8);
}

// not writeable, i believe, so only one.
void tlb_config_print(void) {
    cp15_tlb_config_t c = cp15_tlb_config_rd();
    printk("TLB config:\n");
    printk("\t%s\n", c.unified_p ? "unified" : "seperate I/D");
    printk("\tlockable data ent=%d\n", c.n_d_lock);
    printk("\tlockable inst ent=%d\n", c.n_i_lock);
}

// decode <u> into a cache size structure.
struct cache_size mmu_cache_size(unsigned u) {
    struct cache_size s;

    // b6-15
    s.len = u&0b11;
    s.m = (u >> 2)&0b1;
    s.assoc = (u >> 3) & 0b111;
    s.size = (u >> 6) & 0b1111;
    s.p = (u >> 11)&0b1;
    return s;
}

// can replace these switch statements with calculation.
void mmu_cache_print(const char *msg, struct cache_size s) {
    printk("%s:\n", msg);
    // printk("\tlen=%d\n", s.len);
    printk("\tm=%d\n", s.m);

    unsigned K = 0;
    switch(s.size) {
    case 0b00: 
    case 0b01: 
    case 0b10: 
    case 0b11: 
        panic("too small\n");

    case 0b100: K = !s.m ? 8 : 12; break;
    case 0b101: K = !s.m ? 16 : 24; break;
    case 0b110: K = !s.m ? 32 : 48; break;
    case 0b111: K = !s.m ? 64 : 96; break;
    case 0b1000: K = !s.m ?128 : 192; break;
    default:
        panic("invalid size\n");
    }
    printk("\tcache size=%dK\n", K);

    unsigned W = 0;
    switch(s.len) {
    case 0: W = 2; break;
    case 1: W = 4; break;
    case 2: W = 8; break;
    case 3: W = 16; break;
    default: panic("invalid len\n");
    }
    printk("\tcache line length = %d words\n", W);

    unsigned A = 0;
    switch(s.assoc)  {
    case 0: A = !s.m ? 1 : 0; break;
    case 1: A = !s.m ? 2 : 3; break;
    case 2: A = !s.m ? 4 : 6; break;
    case 3: A = !s.m ? 8 : 12; break;
    case 4: A = !s.m ? 16 : 24; break;
    default: panic("not handling assoc=%d\n", s.assoc);
    }
    printk("\tassoc = %d-way\n", A);
    if(s.p)
        printk("\tpage allocation restriction\n");
    else
        printk("\tpage allocation not restricted\n");
}

/*************************************************************************
 * tlbr  b4-41
 */
void ttbr0_get(unsigned *base, unsigned *rgn, unsigned n) {
    assert(n == 0);
    // assert(cp15_ttbr_ctrl_rd() == 0);

    unsigned r = cp15_ttbr0_rd().base;
    *rgn = (r >> 3) & 0b11;
    *base = (r >> 14) << 14;

}

void ttbr1_get(unsigned *base, unsigned *rgn, unsigned n) {
    assert(n == 0);
    //assert(cp15_ttbr_ctrl_rd() == 0);

    unsigned r = cp15_ttbr1_rd().base;
    *rgn = (r >> 3) & 0b11;
    *base = (r >> 14) << 14;
    // assert(*base == 0);
}

void cp15_tlbr_print(void) {
    unsigned base0, rgn0, base1, rgn1;
    ttbr0_get(&base0, &rgn0, 0);
    ttbr1_get(&base1, &rgn1, 0);
    printk("TLBR registers:\n");
    printk("\ttlb base control: N=%b\n", cp15_ttbr_ctrl_rd());
    printk("\tTTBR0: base=%x, rgn=%b\n", base0, rgn0);
    printk("\tTTBR1: base=%x, rgn=%b\n", base1, rgn1);
}


/*************************************************************************
 * domain access control  b4-42
 */

void cp15_domain_print(void) {
    printk("domain access control:\n");
    printk("\t%b\n", cp15_domain_ctrl_rd());
}

#if 0
void domain_acl_set(unsigned domain_id, unsigned perm) {
    // we aren't doing anything fancy.
    assert(domain_id == 0);

    // always
    assert((perm & 0b11) == perm);
    assert(perm == 0b11 || perm == 0b01);
    assert(domain_id < 16);

    // unneeded, but this is what we'd do w/ more domains.
    unsigned u = read_domain_access_ctrl();
    u &= ~(0b11<<domain_id);
    u |= perm << domain_id;

    write_domain_access_ctrl(u);
}
#endif

/**********************************************************************
 * ASID code
 */
struct tlb_procid {
    unsigned asid:8,
             pid:24;
};

void mmu_get_curpid(unsigned *pid, unsigned *asid) {
    unsigned procid = cp15_procid_rd();
    *pid = procid >> 8;
    *asid = procid & 0xff;
}

void mmu_asid_print(void) {
    unsigned pid, asid;
    mmu_get_curpid(&pid,&asid);
    printk("tlb procid = %x,  asid = %d\n", pid, asid);
}
