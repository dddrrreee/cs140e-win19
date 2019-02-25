## Simple virtual memory.

Today you'll get a simple "hello world" version of virtual memory working.
This and the SD card file system are the biggest unknowns in our universe,
so we'll bang out quick versions of each, and then circle back around
and make your system more real.

#### The big picture

#### Lab reading.

This is one of those labs where I probably should have given your
pre-lab reading, but we'll see if we can get through this without it.
The fundamental thing we are doing is not that complicated; though there
are a fair number of details.

The main thing you'll need for this lab: I've gone through about 100
pages of the ARM manual and annotated it.  No guarantee that these
are all the annotations necessary, but they definitely cover stuff you
should understand.  In the `./docs` directory for this lab:
   1. `armv6.b2-memory.annot.pdf`: section B2 of the ARM manual, 
   describing memory ordering requirements ---
   what you have to do when you update the page table, the page table
   registers, etc.

   2. `armv6.b3-coprocessor.annot.pdf`: section B3 of the ARM manual,
   describing the different co-processor options.

   3. `armv6.b4-mmu.annot.pdf`: section B4 of the ARM manual, describing
   the page table format(s), and how to setup/manage hardware state for
   page tables and the TLB.

#### Check-off

You need to show that:
  1. You can replace the code to setup the page table with 1MB sections.
  2. You can replace the code to synchronize the hardware state.

Extensions:
   1. Setup two-level paging.
   2. Catch segmentation faults.

----------------------------------------------------------------------
### Part 0: make sure you can run the simple hello program (5 minute).

This is just a quick debug that your system is working fine.

----------------------------------------------------------------------
### Part 1: implement the code to setup page tables using 1MB sections (45 min)

You'll write the code for `mmu_setup()`.
You'll use 1MB sections, these are described in:

Once this is working you have legitimate virtual memory.
  1. Write a test case that shows you get a fault when you reference
  unmapped memory.

  2. Write a test case that maps an address range to a different 
  one and test that its working.

The document you'll need for this part is:
  * The annotated B4 of the ARM manual `docs/armv6.b4-mmu.annot.pdf`,
  which describes the page table format(s), and how to setup/manage
  hardware state for page tables and the TLB.

Useful pages:
  - `S`, `R`, `AXP`, `AP` defined on B4-9.
  - Domain permissions B4-10.
  - `C`, `B`, `TEX` encodings B4-12.
  - `nG`, `ASID`, `S`, 'XN`: B4-25.
  - layout of "section" page table entry (PTE): B4-27. 
  - bit[18] (the IMP bit) `IMP = 0` for 1MB sections B4-28
  - translation of a 1MB section: B4-29.

We can break this down into two pieces:
  1. Write the page table format.
  2. Set the page table state.
  3. Swap.


The following screenshots are taken from the B4 section, but we inline
them for easy reference:

The PTE for 1MB sections document:
<table><tr><td>
  <img src="images/part1-section.png"/>
</td></tr></table>

The definitions for `S`, `R`, `AXP`, `AP`:
<table><tr><td>
  <img src="images/part1-s-r-axp-p.png"/>
</td></tr></table>

The definitions for `TEX`, `C`, `B`:
<table><tr><td>
  <img src="images/part1-tex-C-B.png"/>
</td></tr></table>


----------------------------------------------------------------------
### Part 2: Handle initialization (45 min)

Weirdly, this is --- by far --- the hardest part to get right:
  1. One reason: if you get it wrong, your code may "work" fine. We are 
  running with caches disabled, no branch prediction, and strongly-ordered
  memory accesses so many of the gotcha's can't come up.  However, later,
  they will.  And since at that point there will be more going on, it
  will be hard to figure out WTH is going wrong.

  2. Because flaws relate to memory --- what values are returned from
  a read, or what values are written --- they give "impossible" bugs
  (e.g., a write to a location disappears despite you staring right at
  the store that does it, a branch is followed the wrong way despite
  its condition being true).  They are the ultimate memory corruption,
  but much fancier.

So for this part, like the `uart` lab, you're going to have to rely very
strongly on the documents from ARM and find the exact prose that states
the exact sequence of (oft non-intuitive) actions you have to do.

Mostly you'll find these in:

   * Section B2 of the ARM manual (`docs/armv6.b2-memory.annot.pdf`)
   describing memory ordering requirements --- what you have to do when
   you update the page table, the page table registers, etc.

-----------------------------------------------------------------------
### Further reading

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

