---
layout: page
title: Quick overview.
show_on_index: true
---

### Lab: make sure R/pi hardware and its toolchain works.

There's a lot of fiddly little details in getting a pi working, and this
is many people's first time working with this kind of hardware, so we
break this down into many (perhaps too-many) small steps.  Please help
make this document better!

We'll use different variations of a blinky light using GPIO pin 20:

  1. you'll make the LED turn on manually;
  2. then use a pre-compiled program (why not skip 1?);
  3. then use a bootloader (why not skip 2?);
  4. then install the r/pi tool chain, compile a given assembly
	   version and use it (why not skip 3?);
  5. then write your own and compile: this is the fun part.  It's also
	the longest (why not skip 4?).

The final sign off for the lab, show the TA/me your code and run four
  programs: 
  1. one that blinks pin 20; 
  2. pin 16 (right above 20);
  3. pin 5;
  4. Finally, blink 20 and 21 (right below 20) simultaneously.
  (This will point out a subtle mistake people make reading the docs).

Also, with your partner: write out any problems you hit, how you diagnosed
them, what the solution was, and post to the newsgroup.  This will be
useful later.

#### 0. Make sure you have everything.

Pick up:
  1. a R/PI A+;
  2. a microSD card and adapter;
  3. a CP2102 USB-TTL adapter;
  4. however many led's you want;
  5. some female-female jumpers;
  6. printout of the pi's pins.

#### 1. Make sure hardware is working:
  Use the USB-TTY to power the pi, and use the pi's power to directly
  turn on an LED.  This tests some basic hardware and that you know how to
  wire.

  Mechanically:
  1. Connect the USB-to-TTL Serial cable's power (red) and ground  (black)
     wires to the 5v and ground pins on the pi that are next to each other
	(see your printout; upper right corner).
  2. Plug the USB-TTY into your USB port.
  3. Connect  your LED up to another ground pin and a power pin (there are
  several) to 
   make sure the hardware components work, and you know how get them to.
  If the LED doesn't go on, reverse its connections.  
	If still doesn't go on, plug someone
  else's working version into your computer.  If that doesn't work, ask.

  (EE folks:
  We don't use a breadboard b/c it's bulky; we don't use resistors for the
  same reason + the LED is big enough we generally don't fry it.)

#### 2.  Make sure you're able to install firmware, etc:

  Copy the precompiled program `part1/blink-pin20.bin` to the SD card as
  `kernel.img`, put the card in the pi, hook up the LED to pin 20, connect
  the TTY-USB.  After it boots, the pi will jump to whatever code is 
  in `kernel.img` --- in our case, code to turn pin 20 on and off.

  Note: in the next assignment  when you develop your own remote
  bootloader (see next step), if your code is broken you'll need to use
  this SD card method to load a new version, so pay attention to how you do it
  on your computer.

  Mechanically:
  1. Unplug the USB-TTY.
  2. Plug SD card into your computer and figure out where it's mounted.
  3. Copy all the files from the `firmware/` directory onto the SD card.
  4. copy `part1/blink-pin20.bin` to the SD card as `kernel.img`.
  5. unmount the SD card (don't just pull it out!  data may not be written out.)
  6. connect the LED to GPIO20 and ground. 
     Use `docs/gpio.png` to figure out which this is.
  7. Plug the SD card into your pi
  8. plug in the USB-TTY to your USB to power the pi.

It should be blinking.  If you get this working, please help anyone else that
is stuck so we all kind of stay about the same speed.  

Troubleshooting:
   1. If it's not blinking, swap in someone else's card that is working.
   2. If that works, compare their SD card to yours.
   3. If that doesn't work, try your card in their rpi.  


#### 3.  Send a new pi program from your computer rather than SD card.

  As you've noticed, running new programs on the pi using the SD card
  method is tedious.  This step shows makes it so you can send programs
  directly from your computer to a plugged-in pi.

  Method: install a program (which we somewhat inaccurately call a
  "bootloader").   This program will wait on the pi for
  a program sent by your computer, copy it into pi memory, and then
  jump to it.  We currently give you a pre-compiled version (
	`firmware/bootloader.bin`). 
	Our first homework next week will be to write your own.

  Mechanically:
  0. Don't touch the wiring for the LED.
  1. Copy `firmware/bootloader.bin` on your SD card as `kernel.img` (see a 
	pattern?).
  2. Hook the TX and RX wires up to the pi.  Do you TX/TX and RX/RX or
     switch them?  (Hint: Think about the
     semantics of TX (transmit) and RX (receive).)
  3. If you have a mac, first download and install the drivers for a
   CP210x USB-to-UART driver as described in the cs107e docs:
	(http://cs107e.github.io/guides/mac_toolchain/).
	(It's a mac, so make sure you reboot after doing so.)
  3. Either copy `bin/rpi-install.py` to your local `bin/` directory or
    add the absolute path to `cs140e-win19/bin/` to your path.
  4. Run `rpi-install.py part1/blink-pin20.bin`
	(If the command fails, you may need to force the use of python3
  	or refresh your shell's PATH variable).

Your LED should be blinking.

Troubleshooting: 
  1. `sudo pip install {pyserial,xmodem,serial}`
  2. If you use a different serial adaptor, you will have to change the 
   code in rpi-install.py to recognize it.    There are some comments to help.

#### 4.  Make sure your r/pi toolchain is working.

For this class
you need to compile bare-metal r/pi programs on your computer, which is 
most likely
not a bare-metal r/pi itself.  Thus we need to set up the tools needed to
``cross-compile'' r/pi programs on your computer and to r/pi binaries.

Install the toolchain:
   -  For a mac: (http://cs107e.github.io/guides/mac_toolchain/)
   - For ubuntu/linux (from: 
   (https://github.com/eistec/mulle/wiki/Installing-toolchain-%28GCC%29)):

           sudo add-apt-repository ppa:team-gcc-arm-embedded/ppa
           sudo apt-get update
           sudo apt-get install gcc-arm-none-eabi

Compile `part2/blink-pin20.s`

   1. `cd part2`.   Run `make.sh`.
   2. reset your pi: unplug the TTY-USB then plug it back in to your laptop.
   3.  `rpi-install.py blink-pin20.bin` (in `part2/`).  Should blink.  If 
	not isolate the problem by trying the blink-pin20.bin in `part1/`, your lab partner's, etc.

#### 5. write your own blink!

Now we get to the fun part.  You'll read the Broadcom document to see how to 
turn the GPIO pins on yourself and then filling in the code in `part3/blink.c`.

Change the code to first work with GPIO20.

   1. look at the broadcom document: `docs/BCM2835-ARM-Peripherals.pdf`
   pages 90--96.  NOTE: where the broadcom document uses
   addresses `0x7420xxxx`, you'll use `0x2020xxxx`.
   2. Adapt the code in `part3/blink.c` to (1) set GPIO pin 20 to output,
   and then in a loop repeatedly set it on and off ("clear").  
   3. After each change, power-cycle the pi, and use the bootloader to
   load the code. `rpi-install.py kernel.img`

Generalize your code to work with any pin from 0 to 30 (note, not all of these
are defined, but ignore that):  

   1. Note that the different `GPFSELn` registers handle group of 10, so you 
	can divide the pin number to compute the right `GPFSEL` register.
   2. You will be using this code later!   Make sure you test the code by 
	rewiring your pi to use pins in each group.

Hint:

  0.  Be very careful to read the descriptions in the broadcom document to
   see when you are supposed to preserve old values or ignore them.
   If you don't ignore them when you should, you can write back
   indeterminant values, causing weird behavior.  If you overwrite old
   values when you should not, the code in this assignment may work,
   but later when you use other pins, your code will reset them.
 
               // assume: we want to set the bits 7,8,9 in <x> to <v> and
               // leave everything else undisturbed.
               
               x &=  ~(0b111 << 7); // clear the bits 7, 8, 9  in x
               x |= (v << 7);     // or in the new bits
                          

  1. You write `GPFSELn` register (pages 91 and 92) to set up a pin as an
  output or input. You'll have to set GPIO 20 in GPFSEL2 to output.

  2. You'll turn it off and on by writing to the `GPSET0` and `GPCLR0`
  registers on page 95.  We write to `GPSET0` to set a pin (turn it on)
  and write to `GPCLR0` to clear a pin (turn it off).

#### 6. Extra: Break and tweak stuff.

You're going to break and change your code to see effects of things going 
wrong and to make it somewhat better:

   1. Eliminate volatile from your code.  The easiest way: just put
   `#define volatile` at the top of your file.  Recompile using `-O2`.
   What happens?  Why?  (It may help to look at the assembly code 
   in `blink.list` and compare how it changes.)

   2. Change your delay to increasingly smaller amounts.   What is going on?

   3. Change from using `volatile` pointer read/writes to using external
	functions (`get32`, `put32`, `get16`, `put16`, `get8`, `put8` to 
	get or set the indicated number of bits).

	The easiest way to see how:  make a simple file `foo.c` with routines
	that take in a pointer "p" and either read it by returning `*p` or 
	write it via `*p = v`.  You can see the assembly gcc generates
	by using the command:

           gcc -S -O2 foo.c
           cat foo.s

   4. Add the reboot code below (we'll go into what different things mean)
   so that you don't have to unplug, plug your rpi each time:

           // define: dummy to immediately return and PUT32 as above.
           void reboot(void) {
                const int PM_RSTC = 0x2010001c;
                const int PM_WDOG = 0x20100024;
                const int PM_PASSWORD = 0x5a000000;
                const int PM_RSTC_WRCFG_FULL_RESET = 0x00000020;
                int i;
                for(i = 0; i < 100000; i++)
                     dummy(i);
                PUT32(PM_WDOG, PM_PASSWORD | 1);
                PUT32(PM_RSTC, PM_PASSWORD | PM_RSTC_WRCFG_FULL_RESET);
                while(1);
           }
   Change your code to just loop for a small fixed number of times and make
   sure reboot() works.

   5. Force the blink loop to be at different code alignments mod 64.   Do 
   you notice any difference in timing?  (You may have to make your 
   delay longer.)  What is going on?  

#### Additional information

More links:

  1. useful baremetal information: (http://www.raspberrypi.org/forums/viewtopic.php?t=16851)

  2. more baremetalpi: (https://github.com/brianwiddas/pi-baremetal)

  3. And even more bare metal pi: (http://www.valvers.com/embedded-linux/raspberry-pi/step01-bare-metal-programming-in-cpt1)

  4. Finally: it's worth running through all of dwelch's examples:
  (https://github.com/dwelch67/raspberrypi).
