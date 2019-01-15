---
layout: page
title: Implementing a bootloader
show_on_index: true
---

### Overview
	
This class you're going:

	1. Write your own bootloader (both the Unix and pi side)
	to replace what we used last lab (i.e., `rpi-install.py` and
	`bootloader.bin`).

	2. Write your own code to enable the UART and GPIO
	pin functions to replace the code givin in this lab
	(in `bootloader/pi-side/libpi.small/uart.o` and
	`bootloader/pi-side/libpi.small/gpio.o`).


At this point, with the exception of about 20 lines of code, all the
code running on the pi and on the Unix end will have been written by you.

The cool thing about writing all the code is that it guarantees there is
no magic, and you understand everything.    The downside is that if there
is a mistake, the rest of the quarter can be a rough ride.   Thus,
the third, final piece of the homework will be:

	3. Write code that will allow you to cross-check your uart.c
	and bootloader implementations against all those written by
	everyone else.  Once you pass these tests, you will be surprised
	if the code breaks.

The two benefits of part 3 are that: (1) if you pass it, you will almost
certainly get 100% on the assignment since it will catch most bugs,
and (2) you will learn some cool, interesting tricks that we can use
the rest of the quarter and after.


### Part 1: Implementing a fast, simple bootloader.


Look through the code in `bootloader/`.  

	1. The sub-directory `pi-side` has the skeleton code that will run 
	on the pi.  You'll implement the missing code in `notmain`, complile
	it, and copy it to the SD card as `kernel.img` just as before.

	2. The sub-directory `unix-side` has the code that will run on
	your Unix laptop. You'll implement the code missing in `support.c`
	and in `simple-boot.c`.

After you do so, running `my-install blink-pin20.bin` in the `unix-side`
directory should cause an LED  hooked up to pin 20 to blink as before.

For historical reasons, the bootloader we gave you in lab1 uses the
xmodem protocol to send bytes between Unix and pi.  (cs107e still uses
this method.)  
To get a feel for what this looks like, 
you can
look through the `sample-code-xmodem` subidectory in this directory.

The high bit, which is obvious after a few seconds of code inspection,
is that the approach is 
complicated and fragile (the cs107e code has bugs,
and I think this one does too).  So you're going to implement a faster,
simpler boot system by extending the code in `bootloader/pi-side` and
`bootloader/unix-side`.

Unix side: to send a binary program, send the following:

	1. `SOH` : (all these values defined in `shared-code/simple-boot.h`)

 	2. `nbytes` : the size of the file

	3. `checksum` : a 32-bit crc of of the nbytes and code

	4. wait to receive the echoed SOH, a crc32 checksum of the nbytes,
	and the checksum back.  If you don't get it, exit with an error.

	5. the binary code:   Copy it starting at 0x8000 using the original
	bootloader code.

	6. `EOT`.

	7. Wait for an `ACK`.

	8. Done.

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

### Advanced.

When you get the code working:

   	1. change it to use a faster bitrate --- at what point does it 
       	   stop working?  You'll have to modify the constant 270 in 
	   librpi/uart.c to make the pi recieve faster.

	2. You may want to try enabling the cache (enable_cache()) to 
	see how much faster you can push it.

### Part 2: Implementing the UART code.

TBD.

You'll go through the broadcom documents and implement the code to initialize
the bootloader.  The next lab will go over this.

### Part 3: Cross-checking your UART and GPIO against everyone else.

TBD.

You'll write a simple system to log each `get32` and `put32` to device
memory and verify that the addresses you read and write, the order you
do so, and the final values written are identical to everyone else in the class.
By checking equivalance we know that if one 
person gets it right, and everyone is equivalant, that everyone is right.
Monday's lab 4 will be over this part.
