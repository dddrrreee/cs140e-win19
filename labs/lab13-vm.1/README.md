## Hooking translation up to hardware

Last lab we did the page table, the main noun of the VM universe.  This
lab we do the main gerunds needed to hook it up to the hardware: 

 - setting up domains.
 - setting up the page table register and ASID.
 - making sure the state is coherent.  

You'll write assembly helper routines implement these (put them
in `vm-asm.s`).  Mechanically, you will go through, one-at-a-time
and replace every function prefixed with `our_` to be your own code.
The code is setup so that you can knock these off one at a time, making
sure that things work after each modification.

#### Check-off

You need to show that:
  - Part 0: you can set up domains.  You handle flushing correctly
  (with page table citations).  You can make execution fault when you
  write to a read-only address.
  - Part 1: you can switch the ASID and page-table pointer.  You handle
  coherence correctly (with page number citations). 
  - Part 2: turn on/turn off the MMU.  You handle coherence / flushing
  corectly (with page number citations).  You should extend this to handle
  caching.
  - Delete all of our files and starter code (remove references from the
  `Makefile`).  At this point, all code is written by you!

Extensions:

  0. Figure out how to enable `XN` checking.
  1. Set-up two-level paging.
  2. Set-up 16MB paging.

----------------------------------------------------------------------
## Part 1: setting up domains.

Deliverables:
  1. You should replace `our_write_domain_access_ctrl` with yours.
  Make sure you obey any requirements for coherence stated in Chapter B2,
  specifically B2-24.  Make sure the code still works!

  2. Change the domain call so that permission checking works and that
  we now get a failt when we write to a location we do not want to allow
  code to (1) execute (see the XN bit) or (2) write to.

Useful pages:
  - B4-10: what the bit values mean for the `domain` field.
  - B4-27: the location / size of the `domain` field in the segment 
  page table entry.
  - B4-42: setting the domain register.


#### Some intuition and background on domains.

ARM has an interesting take on protection.  Like most modern architectures
it allows you to mark pages as invalid, read-only, read-write, executable.
However, it gives you a way to quickly disable these restrictions in a
fine-grained way through the use of domains.

Mechanically it works as follows.
  - each page-table entry (PTE) has a domain that the entry belongs to.

  - the sytem control register (CP15) has a domain register (`c3`, page
  B4-42) that states whether each of the 16 domains is:

    - no-access (`0b00`): cannot load or store to any virtual address
    controlled by the PTE;

    - a "client" (`0b01`): all accesses must be consistent with PTE
    permissions;

    - a "manager" (`0b11`): no permission checks are done, can read or
    write any virtual address in the PTE region.

As a result, you can quickly do a combination of both removing all access
to a set of regions, and give all access to others by simply writing a
32-bit value to a single coprocessor register.

To see how these pieces play together, consider an example where code
with more privileges (e.g., the OS) wants to run code that has less
privileges using the same address translations (e.g., a device driver
it doesn't trust).
   - The OS assigns the device driver a unique domain id (e.g., `2`).
   - The OS tags all PTE entries the driver is allowed to touch with `2`
   in the `domain` field.
   - When the OS is running it sets all domains to manager (`0b11`) mode
   so that it can read and write all memory.
   - When the OS wants to call the device driver, it switches the state of
   domain `2` to be a client (`0b01`) and all other domains as no-access
   (`0b00`).

Result:
  1. When the driver code runs, it cannot corrupt any other kernel memory.
  2. Switching domains is fast compared to switching page tables (the
  typical approach).  
  3. As a nice bonus: All the addresses are the same in both pieces of
  code, which makes many things easier.

In terms of our data structures: 
  - Assume we gave the driver access to the single virtual segment range
  `[0x100000, 0x20000)`, which is segment 1 (`0x100000 >> 20 = 1`).
  
  - We would set the `domain` field for the associated page table entry 
  containing the first level descriptor to `2`: i.e., `pt[1].domain = 2`.

  - Before starting the device driver we would write `0b01 << 2` into
  register 3 of CP15.   I.e., domain 2 is in client mode, all other
  domains (`0, 1, 3..15`) are in no-access mode.


----------------------------------------------------------------------
##### Bits to set in Domain
<table><tr><td>
  <img src="images/part2-domain.png"/>
</td></tr></table>

----------------------------------------------------------------------
----------------------------------------------------------------------
## Part 1: Implement `set_procid_ttbr0`

You will setup the page table pointer and address space identifier:

  1. The hardware has to be able to find the page table when there is
  a TLB miss.  You will write the address of the page table to the page
  table register `ttbr0`.  Note the alignment restriction!

  2.  The ARM allows each TLB entry to be tagged with an address space
  identifier so you don't have to flush when you switch address spaces.
  Set the current address space identifier (pick a number between
  `1..63`).

  3. You need to read B2 to see the coherence requirements for both.  It's
  not trivial.

  4. Replace `our_set_procid_ttbr0` with yours.  Make sure you can switch
  between multiple address spaces.

----------------------------------------------------------------------
##### Setting page table pointer.

<table><tr><td>
  <img src="images/part2-control-reg2-ttbr0.png"/>
</td></tr></table>



----------------------------------------------------------------------
----------------------------------------------------------------------
## Part 2: implement `mmu_enable` and `mmu_disable`

Now you can write the code to turn the MMU on/off.

The exact sequence is given below.  (See below) Your code should be
`mmu_enable`.  You will have to also: flush the D/I cache, the TLB,
the prefetch buffer, and the wait for everything.  You'll have to look
at Part 3 for the description.  Sorry!

----------------------------------------------------------------------
##### Protocol for turning on MMU.

<table><tr><td>
  <img src="images/part2-enable-mmu.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### Bits to set to turn on MMU

<table><tr><td>
  <img src="images/part2-control-reg1.png"/>
</td></tr></table>


----------------------------------------------------------------------

## Part 3: Flush stale state. (30 minutes)

The previous code (hopefully) works, but is actually incorrect.  We are
getting away with the fact that we are not running uncached, and we
haven't been switching between address spaces.

Weirdly, this is --- by far --- the hardest part to get right:
  1. If you get it wrong, your code may "work" fine. We are running with
  caches disabled, no branch prediction, and strongly-ordered memory
  accesses so many of the gotcha's can't come up.  However, later,
  they will.  And since at that point there will be more going on,
  it will be hard to figure out WTH is going wrong.

  2. Because flaws relate to memory --- what values are returned from
  a read, or what values are written --- they give "impossible" bugs
  that you won't even be checking for, so won't see.  (E.g., a write to a
  location disappears despite you staring right at the store that does it,
  a branch is followed the wrong way despite its condition being true).
  They are the ultimate memory corruption, but much fancier.

So for this part, like the `uart` lab, you're going to have to rely very
strongly on the documents from ARM and find the exact prose that states
the exact sequence of (oft non-intuitive) actions you have to do.

Mostly you'll find these in:

   * Section B2 of the ARM manual (`docs/armv6.b2-memory.annot.pdf`)
   describing memory ordering requirements --- what you have to do when
   you update the page table, the page table registers, etc.

Useful pages:
  - B6-22: all the different ways to flush caches, memory barriers (various
    snapshots below).  As you figured out in the previous lab, the r/pi A+
    we use has split instruction and data caches.
  - B2-23: how to flush after changing a PTE.
  - B2-24: must flush after a CP15.
  - B2-25: how to change the address space identifier (ASID). 

----------------------------------------------------------------------
##### When do you need to flush 

<table><tr><td>
  <img src="images/part3-tlb-maintenance.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### Sync ASID

<table><tr><td>
  <img src="images/part3-sync-asid.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### When to flush BTB

<table><tr><td>
  <img src="images/part3-flush-btb.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### How to invalidate after a PTE change

<table><tr><td>
  <img src="images/part3-invalidate-pte.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### Invalidate TLB instruction

<table><tr><td>
  <img src="images/part2-inv-tlb.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### DSB, DMB instruction

<table><tr><td>
  <img src="images/part3-dsb-dmb.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### Flush prefetch buffer and others

<table><tr><td>
  <img src="images/part3-flushprefetch.png"/>
</td></tr></table>


-----------------------------------------------------------------------
#### Lab reading.

The main documents in the `docs/` directory:

   1. `armv6.b2-memory.annot.pdf`: section B2 of the ARM manual,
   describing memory ordering requirements --- what you have to do when
   you update the page table, the page table registers, etc.

   2. `armv6.b3-coprocessor.annot.pdf`: section B3 of the ARM manual,
   describing the different co-processor options.

   3. `armv6.b4-mmu.annot.pdf`: section B4 of the ARM manual, describing
   the page table format(s), and how to setup/manage hardware state for
   page tables and the TLB.

### Further reading

As an alternative to our lab writeup:
 * [Concise, concrete pi MMU](https://github.com/naums/raspberrypi/blob/master/mmu/README.md)

Useful code (use to double-check understanding):
 - [Linux TLB code for V6](https://elixir.bootlin.com/linux/latest/source/arch/arm/mm/tlb-v6.S).
 - [Linux cache code for V6](https://elixir.bootlin.com/linux/latest/source/arch/arm/mm/cache-v6.S).
 - [MMU code for vmwos](https://github.com/deater/vmwos/blob/master/kernel/memory/arm1176-mmu.c)
 - [PiOS source](https://www.stefannaumann.de/git/snaums/PiOS/src/branch/master/source)

To re-affirm your grasp of virtual memory, the slides from 
[CS140 lecture notes](http://www.scs.stanford.edu/19wi-cs140/notes/) give
 a big picture overview:
  1. [Virtual memory hardware](http://www.scs.stanford.edu/19wi-cs140/notes/vm_hardware-print.pdf).
  2. [Virtual memory OS](http://www.scs.stanford.edu/19wi-cs140/notes/vm_os-print.pdf)

And for more detail, the book [Operating systems in three easy pieces](http://pages.cs.wisc.edu/~remzi/OSTEP/) provides chapters online.  You want to look at:
  1. [Address spaces](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-intro.pdf).
  2. [Address translation](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-mechanism.pdf).
  3. [Translation lookaside buffers](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-tlbs.pdf).
  4. [Complete VM systems](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-complete.pdf).


