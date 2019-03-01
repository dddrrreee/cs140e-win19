## Some final projecct possibilities.

Final projects:
  - Do something interesting!
  - 1-4 people: the more people, the more you better do.
  - You can spend $40 or so without asking (we'll reimburase) but if you
  need more check first.
  - Presentations will be during the final exam slot.  

You can always devise your own final project but I've typed out some
possible options. 

### Porting

We use the `r/pi A`+, put there are many little boards out there. 
An interesting project is porting a bunch of the different code we've
built to another system.  

I have the following boards available:
  - [Teensy 2.8, 3.2](https://www.sparkfun.com/products/13736): fast, small
  - [pocketbeagle](https://beagleboard.org/pocket): smaller than the pi!
  - various [esp8266 boards](https://www.sparkfun.com/products/13678): 
  a low-cost, wifi capable system.
  - [pi zero](https://www.adafruit.com/product/2885): a smaller pi. 

### Other projects

Various short options:
  - Add networking to the pi.  One option is to hook it up to the 
  [esp9266](https://www.sparkfun.com/products/13678) via the uart and
  communicate that way.  Another is to use xbee's.

  - Develop the virtual memory system into one that is full featured, able
  to handle the different page sizes, the different protection bits, etc.

  - Do a simple `FAT32` file system so sensors can write data to the sd
  card and keep it around; make this available using FUSE so your laptop
  can access.  Cooler: use the wireless above to export the SD's FAT32
  file system to your computer remotely using FUSE.

  - Do a trap-based valgrind/purify so you can detect memory corruption.
  Valgrind checks every load and store to see if you are writing outside of
  an object's boundary.  It does so by dynamically rewriting the executable
  code.  This is hard.  Instead you can use your virtual memory system to:
    1. Mark all heap memory as unavailable.
    2. In the trap handler, determine if the faulting address is in bounds.
    3. If so: do the load or store and return.
    4. If not: give an error.
  
  Given how fast our traps are, and how slow valgrind is, your approach
  might actually be faster.

  - Do a trap-based race detector: similar to valgrind above, Eraser
  is a well known (but dead) tool for finding race conditions that worked
  by instrumenting every load and store and flagging if a thread accessed 
  the memory with inconsistent set of locks.  As above, b/c binary rewriting 
  is hard you can use memory traps to catch each load/store and check if
  the locks the current thread holds are consistent.

  - Do a statistical version of either the race detector or memory
  checker above: set your timing interrupts to be very frequent and in the
  handler, do the check above.  It may miss errors, but will be very fast.



### Stupid domain tricks

As discusse lab, you can use ARM domains to very quickly switch the
permissions for a set of pages.    It'd be interesting to use this
ability for various things.  One possibility: use it to make embedded
development less dangerous.  

#### Protected subsystems.

As you probably noticed, we have no protection against memory corruption.
As your system gets bigger, it becomes much much harder to track down which
subsystem is causing everything to crash.  Going all the way to 
UNIX style processes is one attack on this problem, but we can use
ARM do to a lighter weight method:
   - give each domain a unique domain number.
   - tag all memory associated with the domain with its domain number.
   (you may have to have additional domain numbers for pages accessible
   by more than one).
   - before you call into the domain, change the domain register so that
   domain has client privileges, but no other domain is accessible.
   - at this point it can only touch its own memory.

This method is much faster than switching between processes: we don't
have to flush the TLB, caches, or page table.

##### Lightweight protection domains

As a lighter-weight intermediate hack: if we want to proect one thread
from another, we usually throw it in a different address space.  This can
be a lot of overhead.  Instad, tag its memory with a seperate domain ID
and do what we do above.

##### Subpage-protection

If we have some code that is causing corruption but we can't fiure out,
use domains to quickly turn off/on write-permissions for all memory
(or for unrelated memory) and monitor the traps that occur to see if
the code is accessing outside of its region.
