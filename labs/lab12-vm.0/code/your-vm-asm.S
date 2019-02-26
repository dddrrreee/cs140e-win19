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

@ used to clear register before CP15 operation.
#define CLR(reg) mov reg, #0 

@ used to be called "drain write buffer"
@ includes all cache operations.  is a superset of
@ > DMB
#define DSB(Rd)             mcr p15, 0, Rd, c7, c10, 4

#define DMB(Rd)             mcr p15, 0, Rd, c7, c10, 5 

@ must flush the prefetch buffer whenever you change a virtual 
@ mapping (ASID, PTE, etc) since it will have stale instructions.
@
@ if you are doing this, likely have to do a DSB before to make
@ sure whatever invalidation you did has completed.
#define ISB(Rd) PREFETCH_FLUSH(Rd)
#define PREFETCH_FLUSH(Rd)  mcr p15, 0, Rd, c7, c5, 4  

@ must do this after changing any MMU stuff, ASID, etc.
#define FLUSH_BTB(Rd)         mcr p15, 0, Rd, c7, c5, 6

@ need to figure out if have a unified or separate I/D cache/TLB

@ Work-around for bug in ARMv6 if we have seperate I/D.  Taken from:
@   https://elixir.bootlin.com/linux/latest/source/arch/arm/mm/cache-v6.S
@ MUST HAVE INTERRUPTS DISABLED!
@ XXX: patch feedback implies we need this for other operations too?
#define INV_ICACHE(Rd)                                           \
    mov Rd, #0                  ;                                   \
    mcr p15, 0, Rd, c7, c5, 0   ; /* invalidate entire I-cache */   \
    mcr p15, 0, Rd, c7, c5, 0;  ; /* invalidate entire I-cache */   \
    mcr p15, 0, Rd, c7, c5, 0;  ; /* invalidate entire I-cache */   \
    mcr p15, 0, Rd, c7, c5, 0;  ; /* invalidate entire I-cache */   \
    .rept   11                  ; /* ARM Ltd recommends at least 11 nops */\
    nop                         ;                                   \
    .endr                     

#define INV_DCACHE(Rd)      mcr p15, 0, Rd, c7, c6, 0  
#define INV_ALL_CACHES(Rd)  mcr p15, 0, Rd, c7, c7, 0  

@ invalidate TLB entries.
#define INV_ITLB(Rd)        mcr p15, 0, Rd, c8, c5, 0 
#define INV_DTLB(Rd)        mcr p15, 0, Rd, c8, c6, 0 
@ invalidate unified TLB or both I/D TLB
#define INV_TLB(Rd)         mcr p15, 0, Rd, c8, c7, 0

#define SET_ASID(Rd)  mcr p15, 0, Rd, c13, c0, 1
#define SET_TTBR0(Rd)  mcr p15, 0, Rd, c2, c0, 0
#define GET_TTBR0(Rd)  mrc p15, 0, Rd, c2, c0, 0
#define SET_TTBR1(Rd)   mcr p15, 0, Rd, c2, c0, 1

.globl get_cache_type
get_cache_type:
    @@@@  implement @@@@ 
    bx lr
