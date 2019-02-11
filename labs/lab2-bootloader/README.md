---
layout: page
title: Quick overview.
show_on_index: true
---

### Lab: write unix support code for HW1 and get pi communication working.

Our first homework will be building your own bootloader.  After 
completing this lab you will

	1. Have all the unix support code you need for the homework
	implemented.

	2. Be able to do a simple ping-pong test between the pi and
	unix side, which will  debug any OS / hardware / conceptual issues
	you might have.

After this lab you can then write the rest of Part 1 of the homework without
too much issue.

#### 1. Write the unix support code.

You'll write the support code needed by your bootloader for (1) opening the 
device to the tty and (2) reading in the binary program.  Neither is very
hard, we do it in lab to make sure that everyone is up to speed.  If you
finish, make sure to help your partner.

Look in `homeworks/1-bootloader/bootloader/unix-side`.  You will need
to implement the two procedures in `support.c`:

	1. `read_file`: read the file `name` into a buffer and return it,
	write the size of the buffer in `size`.  Note that 
	you should pad the memory so that it is a multiple of 4.

	You'll implement this using `stat()` to get the size, and then
	use `read()` to read the entire file into memory.  (Don't use
	`fread()` unless you set it up to read binary!)
	
	2. `open_tty` will look though your `/dev` directory to find the
	USB device that your OS has connected the TTY-USB to, open it,
	and return the resultant file descriptor

	You should use `scandir` to scan the `/dev` directory.
	(`man scandir`).  The array `ttyusb_prefixes` has the set of
	prefixes we have seen thus far; if your computer uses a different
	one, let us know!

Both of these will be in your homework, so if any function can fail,
check it, and give an error message if it does.

#### 2.  Write code to ping-pong 32-bit words between your laptop and pi.

If you can't reliably send and receive bytes on either side, the bootloader
can't work, so we verify you can do a simple test:

 	1. Change the code in `bootloader/pi-side/bootloader.c` to wait
	for a 32-bit word, increment it, and send it back, until you
	received 0x12345678, at which point you `reboot()`. 

	2. Change the code in `bootloader/unix-side/simple-boot.c` to
	send a 32-bit word, wait for a reply, and check that it's 
	what it sent + 1.   After a while, send 0x12345678 and verify
	that the connection closes.

	3. Run several times to make sure it keeps working.    

	4. Swap your card with your partner and make sure that your code
	works with theirs.

At this point you should be in fine shape to complete part 1 of the homework.

#### 3.  Sign off.

	1. Show us the code from part 1.
	2. Demonstrate that your ping-pong works with your partner.
