---
layout: page
title: Implementing a bootloader
show_on_index: true
---

### Overview
	
This class you're going:

     1. Write your own bootloader (both the Unix and pi side) to replace
     what we used last lab (i.e., `rpi-install.py` and `bootloader.bin`).

     2. Write your own code to enable the UART and GPIO
     pin functions to replace the code given in this lab
     (in `bootloader/pi-side/libpi.small/uart.o` and
     `bootloader/pi-side/libpi.small/gpio.o`).


At this point, with the exception of about 20 lines of code, all the
code running on the pi and on the Unix end will have been written by you.

The cool thing about writing all the code is that it guarantees there is
no magic, and you understand everything.    The downside is that if there
is a mistake, the rest of the quarter can be a rough ride.   Thus,
the third, final piece of the homework will be:

     3. Write code that will allow you to cross-check your `uart.c` and
     bootloader implementations against all those written by everyone
     else.  Once you pass these tests, you will be surprised if the
     code breaks.

The two benefits of part 3 are that: (1) if you pass it, you will almost
certainly get 100% on the assignment since it will catch most bugs,
and (2) you will learn some cool, interesting tricks that we can use
the rest of the quarter and after.

### Part 1: Implementing a fast, simple bootloader.


Look through the code in `bootloader/`.  

	1. The sub-directory `pi-side` has the skeleton code that will run 
	on the pi.  You'll implement the missing code in `notmain`, compile
	it, and copy it to the SD card as `kernel.img` just as before.

	2. The sub-directory `unix-side` has the code that will run on
	your Unix laptop. You'll implement the code missing in `support.c`
	and in `simple-boot.c`.

After you finish writing this code, running `my-install blink-pin20.bin`
in the `unix-side` directory should cause an LED  hooked up to pin 20
to blink as before.

For historical reasons, the bootloader we gave you in lab1 uses the
xmodem protocol to send bytes between Unix and pi.  (cs107e still uses
this method.)  To get a feel for what this looks like, you can look
through the `sample-code-xmodem` subdirectory.

The high bit, which is obvious after a few seconds of code inspection,
is that the xmodem approach is complicated and fragile (the cs107e code
has bugs, and I think this one does too).  So you're going to implement a
faster, simpler boot system by extending the code in `bootloader/pi-side`
and `bootloader/unix-side`.

Unix side: to send a binary program, send the following:

	1. `SOH` : (all these values defined in `shared-code/simple-boot.h`)

 	2. `nbytes` : the size of the file

	3. `checksum` : a 32-bit crc of the code.  (The CRC32
	implementation is in `shared-code/simple-boot.h`.)

	4. wait to receive the echoed SOH, a crc32 checksum of the nbytes,
	and the checksum back.  If you don't get it, exit with an error.

	5. the binary code:   Copy it starting at 0x8000 using the original
	bootloader code.

	6. `EOT`.

	7. Wait for an `ACK`.

	8. Done.   Exit with an informative error if anything fails.

On the pi side you'll do the opposite:

	1. Wait for `SOH`, nbytes (a 4 byte quantity), checksum (a 4-byte
	quantity).

	2. Send back: `SOH`, CRC32(nbytes), and checksum.

	3. Receive nbytes of data, copying it starting at `ARMBASE`.

	4. Assert that you get `EOT`.

	5. Verify the checksum on the data, and if ok, send back an `ACK`,
	and jump to the code at `ARMBASE`.

	6. For all errors, send back the matching error code.

You'll need to write the pi code to receive these values and copy the
binary to the right place.  After compiling you'll have to copy onto
the SD card. 
 
I'd start small: 

      1. echo SOH from pi-bootloader back to the Unix side and make sure
	you receive it correctly.

      2. echo SOH nbytes checksum and verify reception.

      3. copy the code, echo EOT.

      4. jump to the code.


Make sure your bootloader will work with other people's Unix side client,
and vise verse.

### Part 2: Implementing the UART code.

NOTE: Lab4 will cover much of this.

The code to setup and use the miniUART on the pi is the last "major"
piece of the pi runtime that you have not built yourself.

The miniUART on the pi is what talks to your laptop via the TTY-USB
device.  You will write the code to initialize the miniUART to the state
the TTY-USB needs and write `getc` and `putc` routines to receive and
transmit bytes using it, respectively.

Once you finish, all code on the pi (and much on the Unix end) will have
been built by you, with the exception of the trivial `start.s` and some
helper functions.  Also, once your UART code works, you can then start
using `printf` on the pi, since `printf` can use the miniUART to transmit
the bytes it prints.

Concretely, you will implement three routines (skeletons given in 
`homework/1-boot-loader/my-uart/my-uart.c`):
	
	1. `void uart_init(void)`: called to setup the miniUART.
	It should set the baud rate to 115,200 and leave the miniUART
	in its default 8n1 configuration.

	2. `int uart_getc(void)` which blocks until it can read a byte
	from the miniUART, and returns the byte as an integer 
	(do for consistency with `getc`).

	3. `void uart_putc(unsigned c)` which blocks until it can give
	the byte `c` to the miniUART to transmit.

When compiled the resultant `my-uart.o` should function as a drop-in
replacement for the `uart.o` we gave you.     One extremely useful trick:
if things break, you can swap the old `uart.o` in to see if there is a
problem with your code, or somewhere else (e.g., fried hardware).

In order to do this assignment, you'll have to go through the Broadcom
document  to figure out where, what, and why you have to read/write
values.  Again, lab4 will partially cover this.  As with the GPIO, the
upper bits of the broadcom addresses are incorrect for the pi and you
will have to use different ones (in our case `0x2021`).

NOTE: AN IMPORTANT DELIVERABLE FOR THIS PART IS DOCUMENTED CODE.
Almost all device drivers are full of mysterious reads  and writes of
unexplained constants to unexplained locations in unexplained orders.
At best they are hard to understand, at worst wrong, in either case
giving you no
way to check how well the author understood
the device and why they decided to do what they did, in the order they
did it.

You code WILL NOT be like this.  Every single decision you make should
have a page number, and either a direct quote (typed out) or at least a
succinct paraphrase givin the reason for each and every action you do.
That way someone else can read the code and (1) know why you decided to
do something and (2) decide if you were correct (for example, if they
see you are --- possibly unknowingly --- confused about something).
You do not have to replicate the broadcom document in your code: for
this part, a reasonable balance is that there will likely be about as
many tokens in your comments as in your code.  Of course, if you start
putting long quotes, which is a reasonable thing to do, this will skew
more towards comment length.

Think of your comments as an informal proof sketch stating the reasons
that, QED, your believe your code does what it purports to do.

### Part 3: Cross-checking your UART and GPIO against everyone else.

NOTE: you will have done much of this part in Lab 3.

You'll write a simple system to log each `get32` and `put32` to device
memory and verify that the addresses you read and write, the order you
do so, and the final values written are identical to everyone else in
the class.  By checking equivalence we know that if one person gets it
right, and everyone is equivalent, that everyone is right.

In the lab you have already checked `gpio_set_output`, `gpio_clear`,
and `gpio_set`.  You will extend your checking for your miniUART code.

NOTE: we are in flux as to the right directory structure and makefiles,
so for the moment, run your `uart` routines manually by just dropping
them into your lab 2 code.  Check against your lab partner and use the
newgroup to check against everyone else.
