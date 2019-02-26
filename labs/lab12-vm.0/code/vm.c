/*
	simple code for VM: starter code at the top (mostly).  
    - your code at the end.  
    - write your own implementation of any our_* function.

    b4-6: before mmu enabled, the icache should be disabled and invalidated.
     icache can then be enabled at the same time as the mmu is enabled.
     clean_inv_caches();

    b4-7: strongly recommended that the code which enables or disables the 
     mmu has identical virtual and physical addresses.
*/
#include "rpi.h"
#include "vm.h"

/************************************************************************
 ************************************************************************
 * Starter code.
 */

// check bitfield positions.
#define check_bitfield(T, field, off, nbits) do {                       \
        union _u {                                                      \
                T s;                                                    \
                unsigned u;                                             \
        } x = (union _u) { .s = (T){ .field = 1 } };                    \
        unsigned exp = 1<<off;                                          \
        if(x.u != exp)                                                  \
                panic("offset wrong: expect %x got %x %s [off=%d,nbits=%d]\n", \
                                          exp,x.u,#field,off,nbits);        \
        assert(x.u == exp);                                             \
                                                                        \
        unsigned shift = 32 - nbits;                                \
        unsigned set_all = (~0UL << shift) >> shift;                      \
        exp = set_all << off;                                           \
        x.s.field = ~0;                                                 \
        if(x.u != exp)                                                  \
                panic("width wrong: expect %x got %x %s [off=%d,nbits=%d]\n", \
                                          exp,x.u,#field,off,nbits);        \
        assert(x.u == exp);                                             \
} while(0)

#define print_field(x, field) do {                   \
        printk("\t0b%b\t= %s\n", (x)->field, #field);   \
        if(((x)->field) > 8)                            \
            printk("\t0x%x\n", (x)->field);             \
} while(0)

#define is_aligned(x, a)        (((x) & ((typeof(x))(a) - 1)) == 0)


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
    assert(sizeof(struct control_reg1) == 4);
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
    assert(sizeof(struct tlb_config) == 4);
    check_bitfield(struct tlb_config, unified_p,    0,      1);
    check_bitfield(struct tlb_config, n_d_lock,     8,      8);
    check_bitfield(struct tlb_config, n_i_lock,    16,      8);
}
struct tlb_config get_tlb_config(void) {
    return read_tlb_config();
}
void tlb_config_print(struct tlb_config *c) {
    printk("TLB config:\n");
    printk("\tunified=%d\n", c->unified_p);
    printk("\tlockable data ent=%d\n", c->n_d_lock);
    printk("\tlockable inst ent=%d\n", c->n_i_lock);
}

/*************************************************************************
 *************************************************************************
 * the rest is your code.
 */

/*************************************************************************
 * part0
 */

struct cache_size { 
    unsigned char len, m, assoc, size, p;
};

// implemement this.
static struct cache_size get_size(unsigned u) {
    unimplemented();

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
static void cache_size_print(const char *msg, struct cache_size s) {
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

static void part0(void) {

    printk("***************** part 0 START *********************\n");
    unsigned u = get_cache_type();

    printk("cache type: %b\n", u);
    assert(u & (1<<24));
    printk("I/D cache\n");

                   // 109876543210  12 bits
    unsigned mask = 0b111111111111;
    struct cache_size isize = get_size(u & mask);
    struct cache_size dsize = get_size((u >> 12) & mask);
    cache_size_print("instruction cache", isize);
    cache_size_print("data cache", dsize);
    printk("***************** part 0 END *********************\n");
}

/************************************************************************
 * Part1: implement FLD.
 */

fld_t fld_mk(void) {
    fld_t f;
    // all unused fields can have 0 as default.
    memset(&f, 0, sizeof f);
    f.tag = 0b10;
    return f;
}

void fld_check_valid(fld_t *f) {
    assert(f->_sbz1 == 0);
    assert(f->tag == 0b10);
    assert(f->S == 0);
    assert(f->IMP == 0);
    assert(f->C == 0);
    assert(f->B == 0);
    assert(f->super == 0);
}

void fld_check(void) {
    fld_t f = fld_mk();
    fld_check_valid(&f);

    assert(sizeof f == 4);

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

void fld_set_base_addr(fld_t *f, unsigned addr) {
    demand(is_aligned(addr,20), addr is not aligned!);

    // unimplemented();
    f->sec_base_addr = addr >> 20;

    // if the previous code worked, this should always succeed.
    assert((f->sec_base_addr << 20) == addr);
}


/*************************************************************************
 * tlbr  b4-41
 */
#if 0
void ttbr_set(struct tlb_reg *r, unsigned base, unsigned rgn, unsigned n) {
    assert(n == 0);
    assert(is_aligned(base,14));
    r->base = base | (rgn << 3);
    
}
#endif

void ttbr0_get(unsigned *base, unsigned *rgn, unsigned n) {
    assert(n == 0);
    // assert(read_tlb_base_ctrl() == 0);

    unsigned r = read_ttbr0().base;
    *rgn = (r >> 3) & 0b11;
    *base = (r >> 14) << 14;

}

void ttbr1_get(unsigned *base, unsigned *rgn, unsigned n) {
    assert(n == 0);
    //assert(read_tlb_base_ctrl() == 0);

    unsigned r = read_ttbr1().base;
    *rgn = (r >> 3) & 0b11;
    *base = (r >> 14) << 14;
    // assert(*base == 0);
}

static void tlbr_print(void) {
    unsigned base0, rgn0, base1, rgn1;
    ttbr0_get(&base0, &rgn0, 0);
    ttbr1_get(&base1, &rgn1, 0);
    printk("TLBR registers:\n");
    printk("\ttlb base control: N=%b\n", read_tlb_base_ctrl());
    printk("\tTTBR0: base=%x, rgn=%b\n", base0, rgn0);
    printk("\tTTBR1: base=%x, rgn=%b\n", base1, rgn1);
}


/*************************************************************************
 * domain access control  b4-42
 */

void domain_acl_print(void) {
    printk("domain access control:\n");
    printk("\t%b\n", read_domain_access_ctrl());
}

#if 0
void domain_acl_set(unsigned domain_id, unsigned perm) {
    // we aren't doing anything fancy.
    assert(domain_id == 0);

    // always
    assert((perm & 0b11) == perm);
    assert(domain_id < 16);

    // unneeded, but this is what we'd do w/ more domains.
    unsigned u = read_domain_access_ctrl();
    u &= ~(0b11<<domain_id);
    u |= perm << domain_id;

    // since only doing 0
    assert(u == perm);
    our_write_domain_access_ctrl(u);
}
#endif

/**********************************************************************
 * ASID code
 */
struct tlb_procid {
    unsigned asid:8,
             pid:24;
};

static void tlb_get_pid(unsigned *pid, unsigned *asid) {
    unsigned procid = tlb_get_procid();
    *pid = procid >> 8;
    *asid = procid & 0xff;
}

static void tlb_asid_print(void) {
    unsigned pid, asid;
    tlb_get_pid(&pid,&asid);
    printk("tlb procid = %x,  asid = %d\n", pid, asid);
}

// create a mapping for <va> to <pa> in the page table <pt>
void mmu_map_section(fld_t *pt, unsigned va, unsigned pa) {
    assert(is_aligned(va, 20));
    assert(is_aligned(pa, 20));

    // set to the right index in pt.
    fld_t *pte = 0;

    unimplemented();

    fld_print(pte);
    printk("my.pte@ 0x%x = %b\n", pt, *(unsigned*)pte);
}

// would want to initialize everything.
fld_t *mmu_init(unsigned base) {
    demand(is_aligned(base, 14), must be 14 bit aligned!);

    fld_t *pt = (void*)base;
    memset(pt, 0, 4096 * sizeof *pt);

    // should do one-time invalidation of all caches, etc.
    return pt;
}


void part1(void) {
    printk("******** part 1! ************\n");
    unsigned base = 0x100000;

    fld_t *pt = our_mmu_init(base);

    // only mapping a single section: do more.
    our_mmu_map_section(pt, 0x0, 0x0);
    our_mmu_map_section(pt, 0x20000000, 0x20000000);
    our_mmu_map_section(pt, 0x20200000, 0x20200000);

    // this should be wrapped up neater.  broken down so can replace 
    // one by one.

    struct control_reg1 c1 = read_control_reg1();
    c1.XP_pt = 1;
    write_control_reg1(c1);
    c1 = read_control_reg1();
    assert(c1.XP_pt);
    assert(!c1.MMU_enabled);

    our_write_domain_access_ctrl(~0UL);
    dsb();

    // use the sequence on B2-25
    our_set_procid_ttbr0(1, pt);

    // have to flush I/D cache and TLB, BTB, prefetch buffer.
    c1.MMU_enabled = 1;
    our_mmu_enable(c1);
    
    // mmu_enable(c1, pt);

    // VM ON!
    c1 = read_control_reg1();
    assert(c1.MMU_enabled);

    int x = 5, v0, v1;
    v0 = get32(&x);
    printk("doing print with vm ON\n");
    x++;
    v1 = get32(&x);

    // turn off.
    c1.MMU_enabled = 0;
    our_mmu_disable(c1);

    c1 = read_control_reg1();
    assert(!c1.MMU_enabled);
    printk("OFF\n");

    // our reads worked.
    assert(v0 == 5);
    assert(v1 == 6);
    printk("******** success ************\n");
}

/**********************************************************************
 * implement part0, part1, part2.
 */

void notmain() {
    uart_init();
    printk("implement one at a time.\n");
    part1();
    clean_reboot();

    // move about reboot and implement
    part0();

	clean_reboot();
}
