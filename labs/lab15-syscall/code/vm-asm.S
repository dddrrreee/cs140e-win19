@ engler: cs140e.
@
@ Must carefully read B2 of the armv6 manual.  You cannot rely on "well it
@ worked on my test case": mistakes cause stale data, which may or may not
@ show up in your test case (likely not) despite being wildly broken.
@
@ Note: Rd is allegedly a read-only parameter to these instructions, but
@ the manual says SBZ (should be zero).  I think (and the linux source seems
@ to think) that this means we have to explicitly set it to 0.  Not setting
@ Rd=0 is an easy mistake to make.  Perhaps is worth building in (Linux does
@ not, but) --- the cost of an extra mov is negligible given our other 
@ overheads.
@
@ Alot of people put these instructions in inline assembly, wrapped up by 
@ function calls (e.g., cs107e's MMU code).  This practice is dangerous. 
@ For example, if you are flushing caches, you cannot be sure that the 
@ function return, etc does not then reload them, esp w.r.t. the BTB.  I 
@ think the only safe approach is to put directly in assembly so you are 
@ guaranteed no branch, indirect jump, load/store happens in between.
@
@ A lot of MMU code out there appears pretty broken b/c they don't do enough
@ flushing operations and/or rely on gcc code generation to not do the above.

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@ helpers for defining functions in the assembly code.
@

@ used to clear register before CP15 operation.
#define CLR(reg) mov reg, #0 
#define CLRX(macro_name) CLR(r2); macro_name(r2)

@ stronger: wait for the operation to complete.  I think we need a dsb, a 
@ flush btb and a prefetch buffer in general.  though:
@   1. we definitely need a flush_btb if the <macro_name> changes a translation, 
@   permissions, or asid.  otherwise might not.  don't know if worth seperating
@   this case out.
@   2. we need the DSB() after macro name to wait for the cp15 to be visible [cite
@   needed].
@   3. we need the prefetch_flush b/c b2-24: cp15 not vis w/o a prefetchflush.
@   4.  we *do not* need to start with a dsb() to commit outstanding stores, b/c B2 
@   states the coprocessor cannot affect preceeding instructions.  easy to miss this: 
@   Jim pointed it out.
@ go through and re-get the citations.
#define CP15_SYNC(Rz)       \
    FLUSH_BTB(Rz);          \
    CP15_BARRIER(Rz)

#define CP15_BARRIER(Rz)        \
    DSB(Rz);                    \
    PREFETCH_FLUSH(Rz)

#define SYNCX(macro_name, Rz)           \
    CLR(Rz);                            \
    macro_name;                         \
    CP15_SYNC(Rz)

@@@@@@
@ helpers for initiating cp15 operations.
@  MK_FN: just expose the name of the function and insert a return.  don't do any
@         waiting, flushing, sync.
@  FN_SBZ: + clear the temp register before use.  why: it appears that a bunch of 
@          cp15 instructions require that we clear the general-purpose register given as
@           an operand to the instruction even though not actually used (manual says SBZ).
@  FN_SBZ_SYNC: + wait until all effects complete.
@  FN_WR_SYNC:  + use <r0> as cp15 input, <r1> as the sbz temp.
@
#define MK_FN(name, stmts)  \
.globl name;                \
name:                       \
    stmts;                  \
    bx lr
#define FN_SBZ(name, macro_name)  MK_FN(name, CLRX(macro_name))
#define FN_SBZ_SYNC(name, macro_name) MK_FN(name, SYNCX(macro_name(r0), r0))
#define FN_WR_SYNC(name, macro_name) MK_FN(name, SYNCX(macro_name(r0), r1))

@ read's don't need a barrier:  hardware handles.  hardcode r0 as the operand reg
#define FN_RD(name, macro_name) MK_FN(name, macro_name(r0))

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@ cp15 synchronization operations.

#include "arm-coprocessor-insts.h"

FN_SBZ(cp15_sync, CP15_SYNC)
FN_SBZ(cp15_barrier, CP15_BARRIER)

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ weakest: just read from cp15

FN_RD(cp15_ttbr0_rd, TTBR0_GET)
FN_RD(cp15_ttbr1_rd, TTBR1_GET)
FN_RD(cp15_ttbr_ctrl_rd, TTBR_BASE_CTRL_RD)
FN_RD(cp15_domain_ctrl_rd, DOMAIN_CTRL_RD)
FN_RD(cp15_cache_type_rd, CACHE_TYPE_RD)
FN_RD(cp15_tlb_config_rd, TLB_CONFIG_RD)
FN_RD(cp15_ctrl_reg1_rd, CONTROL_REG1_RD)
FN_RD(cp15_ctrl_reg1_rd_u32, CONTROL_REG1_RD)

@ b4-52: set process id (ASID)
@ note: we do not provide a standalone write method: it appears you need to set 
@ ttbr0/ttbr1/config at the same time as the ASID.
FN_RD(cp15_procid_rd, ASID_GET)

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ next weakest: all of these require clearing the temp register, but not waiting.

FN_SBZ(cp15_dsb, DSB)
FN_SBZ(cp15_dmb, DMB)
FN_SBZ(cp15_prefetch_flush, PREFETCH_FLUSH)
FN_SBZ(cp15_btb_flush, FLUSH_BTB)

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ require synchronously waiting for result to finish.
@
@ XXX: i believe: need to figure out if have a unified or separate I/D cache/TLB
@ if unified, I believe cannot do a icache flush, then dcache flush (or vice
@ versa) and get anything useful.

FN_SBZ_SYNC(cp15_caches_inv, INV_ALL_CACHES)
FN_SBZ_SYNC(cp15_dcache_clean_inv, CLEAN_INV_DCACHE)
FN_SBZ_SYNC(cp15_icache_inv, INV_ICACHE)

FN_SBZ_SYNC(cp15_itlb_inv, INV_ITLB)
FN_SBZ_SYNC(cp15_dtlb_inv, INV_DTLB)
FN_SBZ_SYNC(cp15_tlbs_inv, INV_TLB)

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ require synchronously waiting for result to finish, using different register for
@ temp and for input.

FN_WR_SYNC(cp15_ttbr0_wr, TTBR0_SET)
FN_WR_SYNC(cp15_ttbr1_wr, TTBR0_SET)
FN_WR_SYNC(cp15_ttbr_ctrl_wr, TTBR_BASE_CTRL_WR)
FN_WR_SYNC(cp15_domain_ctrl_wr, DOMAIN_CTRL_WR)
FN_WR_SYNC(cp15_ctrl_reg1_wr, CONTROL_REG1_WR)

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ general co-processor operations

@ B6-21 : do after modifying PTE: no, i think have to clean and inv?
@ actually: they better invalidate the icache too, right?  dcache is 
@ insufficient.
@ 
@ actually: i think this has to be bundled with the store, otherwise the
@ random stack loads/stores etc could get messed up!  should make this
@ more precise so it just flushes out the MVA.  yikes: currently crazy
@ expensive.
#define STORE_PTE(Rz)               \
    str r1, [r0];                   \
    CLEAN_INV_DCACHE(Rz);           \
    DSB(Rz);                        \
    INV_TLB(Rz);                    \
    FLUSH_BTB(Rz);                  \
    DSB(Rz);                        \
    PREFETCH_FLUSH(Rz)

FN_SBZ(mmu_sync_pte_mod, STORE_PTE)

@ sequence from b2-25
.globl cp15_set_procid_ttbr0
cp15_set_procid_ttbr0:
    CLR(r2);
    ASID_SET(r2);
    PREFETCH_FLUSH(r2);
    TTBR0_SET(r1);
    TTBR1_SET(r2);
    PREFETCH_FLUSH(r2);
    ASID_SET(r0);
    DSB(r2);            @ wait: we don't need a prefetch flush?
    bx lr

@ one time initialization of the machine state.  cache/tlb should not be active yet
@ so just invalidate.   prefetch flush and btb flush are done by the wrapper.
#define MMU_INIT(Rd)            \
    INV_DCACHE(Rd);             \
    INV_ICACHE(Rd);             \
    INV_TLB(Rd)

FN_SBZ_SYNC(mmu_reset, MMU_INIT)

@ give the control reg: should check that it's set!
.globl mmu_enable_set_asm
mmu_enable_set_asm:
    CLR(r1);
    DSB(r1);                @ make sure all previous stores complete!

    CONTROL_REG1_WR(r0)

    @ at this point i believe:
    @   - our BTB and Prefetch has bad entries.  We are relying on no branches, and a 
    @   previous flush.
    @   - crucial that this code is in the same location both mapped and not mapped.
    @ AFAIK: if we only put these flushes *before* the enable, then we will just
    @ fetch a bunch of stuff that will be wrong after MMU=on.
    FLUSH_BTB(r1);
    DSB(r1);                @ for the enable to be visible.
    PREFETCH_FLUSH(r1);     @ ditto, plus need to flush entries (?)
    bx lr;

.globl mmu_disable_set_asm
mmu_disable_set_asm:
    @ note: this piece of code is mapped into the sample place in both 
    @ MMU-enabled and MMU-disabled so BTB and Prefetch entries work identically
    @ to both MMU-enable and disable.   Without this requirement I believe is
    @ very hard to get right (perhaps impossible?) since we cannot flush
    @ the BTB and prefetch and switch MMU on/off atomically.  thus, would
    @ flush, but instructions could get fetched before we could disable,
    @ defeating the purpose.

    CLR(r1);
    DSB(r1);                @ make sure all previous stores complete!

    @ make sure you clean the data cache before disable, otherwise the entries won't 
    @ be in memory if it was enabled.
    CLEAN_INV_DCACHE(r1);  
    CP15_BARRIER(r1);

    @ disable data cache: will be clean since we have no explicit loads or stores.
    @ [obviously: interrupts must be disabled]
    CONTROL_REG1_RD(r2);
    bic r2, r2, #(1<<2)
    CONTROL_REG1_WR(r2);
    CP15_BARRIER(r1);

    @ 6.4: states you need to disable the data cache before clearing (since otherwise 
    @ can refetch).  However, in this case I think b/c we don't do a load/store, 
    @ Dcache doesn't matter. in any case, we disable just to be sure.  note we did
    @ no loads or stores so far, so dcache should still be clean later.

    @ if we want to disable mmu using R-M-W:
    @   CONTROL_REG1_RD(r0);
    @   /* clear MMU enable bit. */
    @   bic r0, r0, #(1<<0)     
    CONTROL_REG1_WR(r0);
    CP15_BARRIER(r1);

    @ [pretty sure] we need to flush (1) BTB and (2) prefetch at this point
    @ since mapping has changed.  
    @
    @ we do not need to:
    @   - invalidate the dache again since we already did, and the code has not done 
    @     any loads or stores.
    @   - invalidate the tlb: we will have the mmu disabled, so not relevant.  if we
    @   re-enable later, without changing pt then the entries are ok.  if we do change
    @   pt, then we will have cleaned the tlb anyway.
    @
    @ however, we do need to:
    @   - invalidate the icache, since in general it will be full of bad entries (if 
    @   it enabled).  however, the code for this routine is mapped same location mmu=on/off, 
    @   so the current the current program counter is ok.
    @   - wait for the operations to complete.
    INV_ICACHE(r1);         

    @ wait til visible.
    FLUSH_BTB(r1);
    DSB(r1);                @ wait for icache inv to complete.
    PREFETCH_FLUSH(r1);

    @ done!
    bx lr;
