## Make a simple system call.  Make it fast.

Today is a relatively light lab so you have more time for projects.  We're going to 
build a simple system call.  

Once we have this in place, we can then make user-level processes (next
time) --- without it, they would not even be able to call `exit()`.

As a secondary purpose, this lab will get you more comfortable
with interrupts since it will require you do invasive surgery to the 
timer-interrupt code from `lab7-interrupts` to support system calls.

#### Checkoff
  
Checkoffs:
  1. Show that you can handle a system call without virtual memory enabled.
  2. Show that you can handle a system call with virtual memory enabled.
  3. Show that you can add a new system call.
  4. Tune your implementation so it's fast: you should get a 10x speedup with
     a few changes.

### Part0: System calls.

ARM, like many architectures, uses exceptions to dispatch system calls.  You initiate
a syscall exception using the `swi` instruction, which encodes an immediate in its
lower 8-bits.  For example, the instructions `swi 1` and `swi 2` are encoded as follows:

    SWI 0x1            ef000001  
    SWI 0x2            ef000002  

Thus, to figure out which system call code initiated, you would load the instruction
at the program counter  value that the exception happened and mask off the lower 8
bits.

How to write the code:
   1. Put all of your assembly in `code/interrupts-asm.S`.  (The capital
   `.S` at the end tells `gcc` to run the preprocess over the file,
   which makes writing assembly code much more clear).

   2. Steal any useful code you need from
   `lab7-interrupts/timer-int/interupts-asm.s`.

   3. Recall: the `docs/` directory in lab7 has useful ARM information.

The file: `code/driver.c`  has the test code.  You will be writing the
code to make `int_part0()` work.  In particular:
   1. `cpsr_read`: Return the current `cpsr`.
   2. `swi_setup_stack`: setup the stack pointer in `SUPER_MODE` to be `SWI_STACK_ADDR`.
   3. `swi_asm1`: initiate a system call with system call number 
      equal to 1, and the four arguments passed in.  
   4. `software_interrupt_asm`: the trampoline to call `handle_swi` (which we provide).
    This should be similar to `interrupt_asm` in the `timer-int` code from lab7.

   5. Have all other interrupts load `INT_STACK_ADDR` as the stack pointer and
   call the appropriate handler in `interrupts-c.c`.

Make sure you test each piece as you build it!  For example, make sure
the `cpsr` value you return makes sense and that after you write the
`SUPER_MODE` stack, that the value is reasonable.

Don't forget: You can call C code from assembly to print out registers, though you
should reboot doing so since the machine state will likely be trashed.

### Part1: virtual memory

This part will hopefully be fast:
  1. Define the sections you need in `int_part1()`.  The code should just work.  
  2. Add a new system call.

### Part2: make it fast

The time to take a null system call is one of the micro-benchmarks that everyone spends 
time on.  For this part, tune you system call implementation.
  1. save just the state you need.
  2. cut out any extra instructions.
  3. start enabling different caches.

If you don't see a 10x performance improvement, something is wrong, so think about
your code (a few people time the MMU setup or `printk` routines, neither of which
can see much speedup). 
