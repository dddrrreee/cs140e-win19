#ifndef __VM_H__
#define __VM_H__
/****************************************************************************************
 * page table operations: we only handle mapping 1MB sections.
 */


/*
    -----------------------------------------------------------------
    b4-26 first level descriptor  
        bits0:1 =
            - 0b00: unmapped, 
            - 0b10: section descriptor, 
            - 0b01: second level coarse pt.
            - 0b11: reserved

    b4-27 section:
        31-20: section base address: must be aligned.
        19: sbz: "should be zero"
        18: 0
        17: nG:  b4-25, global bit=0 implies global mapping, g=1, process 
                 specific.
        -16: S: 0 deprecated.
        -15: APX  0b11 r/w [see below]
        -14-12: TEX
        -11-10: AP 
        9: IMP: 0: should be set to 0 unless the implementation defined
                   functionality is required.

        -8-5: Domain: b4-10: 0b11 = manager (no perm check), 0b01 (checked perm)
        -4: XN: 1 = execute never,  0 = can execute.
        3:C: set 0 see below
        2:B: set 0
        1: 1
        0: 0

  APX, S, R: 
    b4-8: permissions, table on b4-9
    use of S/R deprecated.

    S  R  APX   AP[1:0]   privileged permissions   user permissions
    x  x    0   0b11       r/w                       r/w
    x  x    0   0b10       r/w                       r/o
    x  x    0   0b01       r/w                       no access
    0  0    0      0       no access                no access
    
  ap,apx,domain:
     b4-8

  c,b,tex:
   b4-11, b4-12 

    TEX   C  B 
    0b000 0  0 strongly ordered.   
    0b001 0  0 non-cacheable


*/

// on pi: organized from 0 to high.
typedef struct first_level_descriptor {
    unsigned
        tag:2,      // 0-1:2    should be 0b10
        B:1,        // 2:1      set to 0
        C:1,        // 3:1      set to 0
        XN:1,       // 4:1      1 = execute never, 0 = can execute
                    // needs to have XP=1 in ctrl-1.

        domain:4,   // 5-8:4    b4-10: 0b11 = manager, 0b01 checked perms
        IMP:1,      // 9:1      should be set to 0 unless imp-defined 
                    //          functionality is needed.

        AP:2,       // 10-11:2  permissions, see b4-8/9
        TEX:3,      // 12-14:3
        APX:1,      // 15:1     
        S:1,        // 16:1     set=0, deprecated.
        nG:1,       // 17:1     nG=0 ==> global mapping, =1 ==> process specific
        super:1,    // 18:1     selects between section (0) and supersection (1)
        _sbz1:1,    // 19:1     sbz
        sec_base_addr:12; // 20-31.  must be aligned.
} fld_t;

// helpers to enable caching / write buffer on a section by section basis. note:
// you must turn on the associated cp15 bits.  
//
// also: do not turn on caching / writeback for GPIO memory!!
#define fld_cache_on(pte) (pte)->C = 1
#define fld_cache_off(pte) (pte)->C = 0
#define fld_writeback_on(pte) (pte)->B = 1
#define fld_writeback_off(pte) (pte)->B = 0

// allocate page table and initialize.  handles alignment.
fld_t *mmu_pt_alloc(unsigned n_entries);

// map a 1mb section starting at va to pa
fld_t *mmu_map_section(fld_t *pt, uint32_t va, uint32_t pa);

// lookup section <va> in page table <pt>
fld_t *mmu_lookup(fld_t *pt, uint32_t va);

// paranoid about flushing state w.r.t. PTE modifications.
void mmu_sync_pte_mod(fld_t *f, fld_t e);

// print single PTE entry.
void fld_print(fld_t *f);

/******************************************************************************
 * mmu functions.
 */

/* call first: one time initialation of caches, tlb, etc */
void mmu_init(void);

// turn on/off mmu: handles all the cache flushing, barriers, etc.
void mmu_enable(void);
void mmu_disable(void);

// reset all caches, tlb, etc.
void mmu_reset(void);

// turn on/off all caches.  should break this down a bit.
void mmu_all_cache_on(void);
void mmu_all_cache_off(void);

#if 0
// same as disable/enable except client gives the control reg to use --- 
// this allows messing with cache state, etc.
void mmu_disable_set(cp15_ctrl_reg1_t c);
void mmu_enable_set(cp15_ctrl_reg1_t c);
#endif

#endif
