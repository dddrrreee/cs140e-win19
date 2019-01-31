

Today's lab:
   1. Extend `my-install` to handle an `-exec <stuff>` argument, where after
   bootloading a pi binary it will hand off the serial file descriptor.
   This is basically just a rip-off of your `handoff.c` code.

   2. Test this code by compiling `echo` in `echo` directory 
   and then running:
	  `my-install -exec ./echo ../test-binaries/hello.bin`.
   This should behave identically to not using `echo`: 
	  `my-install ../test-binaries/hello.bin`.

   3. Write a trivial virtual machine so you can run pi programs on
   your Unix laptop, and just emulate GPIO by sending reads and writes
   to the pi.  Running on Unix will allow you to use `valgrind`, `gdb`,
   or even something as simple memory protection.

This is the Unix side emulation (or virtualization) layer for the pi.
The big picture is that we override low-level functions in `libpi`
and and re-implement them on Unix.

At risk of being overly-dramatic, you are making the virtualization
layer of a  simple virtual machine.
Your pi programs are tiny, trivial operating systems in the power 
sense that
they have complete control over the entire machine (but, of course,
not in the functionality sense of doing fancy OS things).  

To over-simplify, at a mechanical level, a virtual machine is a system
that can take an OS running with kernel privileges on raw hardware and
instead throw it into an unprivileged process and have it run, fast,
and accurately there.

Perhaps counter-intuitively, most OS code can run in an unprivileged
process "as is."  For example, when it performs normal loads, stores,
adds, function calls, etc these run identically both on the bare hardware
in kernel mode and in an unprivileged process.  
The new two main execution issues are:

   1. Handling privileged instructions.

   2. Intercepting when it reads and writes to special memory (e.g., device memory).

Since we haven't covered them yet, our pi code never does privileged
instructions.  Thus, we just need to handle (2), which fortunately for
our code is easy.  Reads and writes to device memory already go through
`put` and `get` so intercepting these just requires linking the code
against our own virtual implementation.  The question then is what to
do?   The semantics of GPIO reads and writes are complicated.  Fortunately
we can entirely side-step this problem and simply run them on the and
report back the results.

The trick:

  1. We provide new implementations of `put`/`get` on the Unix side
  so that we can intercept all reads and writes of device memory.

  2. These implementations simply ship the operation to a small loop
  on the pi, which will perform them.  I.e.,  `put32(addr,val)`
  will send a request to the pi to store to address `addr` with the 
  given value `val`.  The call `get32(addr)` will send a request to 
  the pi to load the 32-bit value at `addr` and return it.

  3. As a result, we do not have to understand the semantics of these
  loads and stores.  In a sense we are using the pi as a perfect simulator
  of what it does.

  4. For the rest of the C code in the pi program you are emulating (but
  not its assembly) we simply recompile it with your laptop compiler
  and link it with a trivial harness that calls `notmain`.

  5. The main problem we need to solve for (4) is that `libpi` code
  provides implementations of core library functions (e.g., `printf`,
  `memcpy`, `putchar`).  We do not want our emulation harness to use
  these, since that can lead to infinite recursion (e.g., for `putchar`)
  or weird behavior.  Our hack is to use `objcopy` to add an `rpi_`
  prefix to all symbols in the pi code, so the pi code can just call its
  implementations, and our emulation harness can call the host versions.
  (You could also get the
  same result by doing pre-processor tricks, but it's uglier.)

The tension in emulation or virtualization is that deciding what
level you want to start doing fake things has a bunch of tradeoffs.
You can push it down to a very low level, so you run more pi code,
or you can raise it, which may be faster, or easier, but is more fake.
For example, consider `printf`.   In our emulation layer we could:

  1. Provide an implementation of `printf` (just call the hosts) and be 
  done with it.  That means we do not run most of the code used by `hello`.

  2. Link in the pi `printf` but provide an implementation of `uart_putc`
  (e.g., by just calling the host `putchar`).
  Lower, runs more code, but still pretty high level.

  3. Do not provide anything.  This is the lowest level, since it will
  run the `uart_init` code, and the rest.  The downside is that we are
  communicating with the pi over the uart, so the `put` and `get` calls in
  the UART will mess with our connection!   We have some choices here.
  The most obvious is to hack the pi-side loop
  to simply discard all writes to UART memory, other
  than those that write data to the transmit or receive data from it.
  We could look for these addresses and return the values that are there.

The tension always arises in virtualization.   Higher-level is usually
easier and faster since you know what the thing is doing and you don't
have to reimplement a bunch of code, but requires more modifications
to the emulated operating system.  For example because you have to
cut out its lower level code and splice in your own or require that
they write some kind of helper code.  (In the virtual machine research,
this approach is called "paravirtualization".)  On the other hand, just
emulating based on reads and writes means you can run more (or all)
of the emulated code, but interpreting the semantics of what a load or
store to device memory actually means is, in general, hard.

I suggest starting with providing `uart_putc` and `uart_init` (which
does nothing) and then possibly working down below that.
