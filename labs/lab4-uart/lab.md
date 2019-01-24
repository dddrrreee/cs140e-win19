---
layout: page
title: Quick overview.
show_on_index: true
---

## Lab: write your own UART implementation.

The code to setup and use the miniUART on the pi is the last "major"
piece of the pi runtime that you have not built yourself.

The miniUART on the pi is what talks to your laptop via the TTY-USB
device.  You will write the code `uart_init()` to initialize the miniUART
to the state the TTY-USB needs and write `uart_getc` and `uart_putc`
routines to receive and transmit bytes using it, respectively.

Once you finish, all code on the pi (and much on the Unix end) will have
been built by you, with the exception of the trivial `start.s` and some
helper functions.  Also, once your UART code works, you can then start
using `printf` on the pi, since `printf` can use the miniUART to transmit
the bytes it prints.

Concretely, you will implement three routines (skeletons given in
`libpi-mine/my-uart.c`):

  1. `void uart_init(void)`: called to setup the miniUART.
  It should set the baud rate to 115,200 and leave the miniUART
  in its default 8n1 configuration.  Before starting, it should
  explicitly disable the uart in case it was already running.

  2. `int uart_getc(void)`: blocks until it can read a byte from the
  miniUART, and returns the byte as a signed `int` (for sort-of consistency
  with `getc`).

  3. `void uart_putc(unsigned c)`: blocks until it can give
  the byte `c` to the miniUART to transmit.

#### What to read.

In order to do this lab, you'll have to go through
Sections 1, 2, 2.1, 2.2, 6.2 in the Broadcom document:  

  * `docs/BCM2835-ARM-Peripherals.pdf` to 
  figure out where, what, and why you have to read/write values.  

  * `docs/BCM2835-ARM-Peripherals.annot.PDF` is a partially annotated 
  version that might be helpful.

  * [errata](https://elinux.org/BCM2835_datasheet_errata) has the
  errata associated with the Broadcom.  

I had to read the sections about 10 times (not kidding).   It's do-able.
Lab will ensure we can get through it to a working program.

##### Helpful notes:

  1. As with the GPIO, the upper bits of the broadcom addresses are
  incorrect for the pi and you will have to use different ones (for
  the miniUART: `0x2021`).

  2.  We don't want: RTS, CTS, auto-flow control, or interrupts, so ignore
  these.

  3. You'll want to explicitly disable the miniUART at the beginning to
  ensure it works if `uart_init(void)` is called multiple times (as can
  happen when we use a bootloader, or `reboot()`).

  4. Similarly, you will often want to fully enable the miniUART's ability
  to transmit and receive as the very last step after configuring it.
  Otherwise it will be on in whatever initial state it booted up in,
  possibly interacting with the world before you can specify how it
  should do so.

  5. Since the device can read or write memory invisibly to the
  compiler, you must manipulate it through volatile pointers or
  use get32/put32/etc.  We will do the latter so that we can do
  additional tricks.

  6. You'll notice the miniUART "register" memory addresses are contiguous
  (table on page 8, section 2.1).  Rather than attempt to type in
  each register's address correctly, just make a large structure of
  `unsigned` fields (which are 32-bits on the pi), one for each entry
  in the table, and cast the miniUART address to a pointer to this
  struct type.  Since you will be using `get32` and `put32` (as before),
  you don't need `volatile`.

  7.  If a register holds received / transmit data you will almost
  certainly want to clear it before enabling the device.
  Otherwise it may hold whatever garbage was in there at bootup.

The main goal of this lab is to try to take the confusing prose and
extract the flow chart for how to enable the miniUART with the baud rate
set to 115200.  You're looking for sentences/rules:

  * What values you write to disable something you don't need
	(e.g., interrupts).

  * What values you have to write to enable things you need (miniUART).

  * How to get the gpio TX, RX pins to do what you need.

  * How to receive/send data.  The miniUART has a fixed sized transmit
	buffer, so you'll need to figure out if there is room to transmit.
	Also, you'll have to detect when data is not there (for `uart_getc`).

  * In general, anything stating you have to do X before Y.

You don't have enough information to understand all the document.  This is
the normal state of affairs.  So you're going to start practicing how
to find what you need and interpolate the rest.

The main thing is to not get too worked up by not understanding something,
and slide forward to what you do get, and cut down the residue until
you have what you need.

## Concrete steps.
==================

`uart-tests` have three programs, numbered by increasing difficulty.

   0. `0char-inf.bin` will print a single character + newline infinitely.
   1. `1hello-inf.bin` will print a `hello world\n` infinitely.
   2. `2hello-once.bin` will print a `hello world\n` once.


Concrete steps:

   0.  Make libpi and all the test programs.  Simply typing `make` in the 
   top level directory (`lab4-uart`) will do so.

   1. Make sure that the three test programs in the `test-uart` work
   by running them using the old bootloader.

   2. Double check that you remember how to use the SD card raw by copying
   `0char-inf.bin` to the SD card (as usual: as `kernel.img`), plug the
   SD card into the pi, and power it up.     To see the output, run
   `pi-cat` (in the `pi-cat`) directory, which will simply print out
   whatever the pi it emitting.

You will put your UART implementation in  the directory `libpie-mine` 
(where the skeleton code is).

  1. Currently running `make` will build `libpi.a` using our UART code.

  2. In order to use your implementation, do not edit the makefile directly.
  If you do, we cannot push updates.  Instead, `Makefile` includes a 
  auxuilary file `manifest.mk` which lists the files to use when building
  `libpi.a`.  You will change it to use your `my-uart.c` rather than 
  our implementation (in the subdirectory `libpi.support`).
  The copy `manifest.mk.my-uart` shows how.   

Now that you verify that everything works with the original system:

	1. Write your own uart.
 	2. Change the libpi manifest as above.
	3. Remake the uart-test programs and run them from the SD card.

## Deliverables
==================

What you should do:

   1. Show us the working `my-uart.c` code.  It should make it clear 
   why you did what you did, and supporting reasons --- i.e., have
   page numbers and partial quotes for each thing you did. 

   2. Show us that `1hello-inf.bin` works when linked with your UART
	implementation.
   3. If your bootloader works: show that it does so with your UART
   and that you can run `2-hello-once.bin`.

Optional: When you pass 1 and 2: let us know and we will give you access
to the most widely-used pi UART code out there.  Surprisingly, it violates
what the Broadcom document states you should do in several ways.  Give us
two examples.

## Extra Credit.
==================

Repurpose the memory tracing code you built in the previous lab to 
check what your UART code produces, and cross-check against your
partner.

You will need to do this for your homework, so might as well do now!
