---
layout: page
title: Quick overview.
show_on_index: true
---

### Lab: make sure hardware and toolchain is working.

We'll use different variations of a blinky light using GPIO 20.

#### 1. Make sure hardware is working:
  1. Connect the USB-to-TTL Serial cable's power (red) and ground  (black)
     wires to the pi.
  2. Plug into your USB port.
  3. Hook your LED up to ground and power to make sure it's wired correctly.
  If it doesn't go on, reverse it.  If still doesn't go on, plug someone
  else's working version into your computer.  If that doesn't work, ask.

#### 2.  Make sure you're able to install firmware, etc:

  1. Unplug the USB-to-pi.
  2. Plug SD card into computer.
  3. Copy the files from firmware/ onto it.
  4. copy part1/blink.bin in this directory to the SD card as kernel.img.
  5. unmount the SD card (don't just pull it out!  data may not be written out.)
  6. connect the LED to GPIO20 and ground. 
     Use docs/gpio.png to figure out which this is.
  7. Plug the SD card into your pi
  8. plug in the USB-to-PI to your USB

It should be blinking.  If you get this working, help anyone else that
is stuck.

Troubleshooting:
   1.  If it's not blinking, swap in someone else's card that is working.
   2. If that works, compare their SD card to yours.
   3. If that doesn't work, try your card in their rpi.  

#### 2.  Use bootloader.

NOTE: If you have a mac, first download and install the drivers for an:
LGDehome PL2303TA USB to TTL.  (Search for PL2303TA, download, reboot.)

  1. Copy bootloader.bin on your SD card to kernel.img.
  2. Hook the white wire from the TTL to pin 14, and the green to 15.
  3. On linux: Run cs49n/bin/rpi-install.py /dev/ttyUSB0 blink.bin
  On mac: Run cs49n/bin/rpi-install.py /dev/usb.serial blink.bin

Things should be blinking.

Troubleshooting: 
  1. sudo pip install {pyserial,xmodem,serial}
  2. If it doesn't find the serial, run "ls -lrt /dev/" after plugging the
   usb-serial in and see what the last device is.
  3. If you use a different serial adaptor, you will have to change the 
   code in rpi-install.py to recognize it.   

#### 3.  make sure your toolchain is working.

Install the toolchain:
   -  For a mac: http://cs107e.github.io/guides/mac_toolchain/
   - For ubuntu/linux:

           sudo add-apt-repository ppa:team-gcc-arm-embedded/ppa
           sudo apt-get update
           sudo apt-get install gcc-arm-none-eabi

   [via https://github.com/eistec/mulle/wiki/Installing-toolchain-%28GCC%29]

Compile blink.s in part2/

   1. Run make.sh
   2. reset pi (unplug, plugin USB cable to your laptop), 
	and use the bootloader to load blink.bin

#### 4. make sure you can read and write GPIO memory.

You'll have to write some code by filling in blink.c in part3/.
First test that the code will blink using GPIO16 (it's the pin above GPI20).  
Change the code to work with GPIO20.

   1. look at the broadcom document: docs/BCM2835-ARM-Peripherals.pdf
   pages 90--96.  NOTE: where the broadcom document uses
   addresses 0x7420xxxx, you'll use 0x2020xxxx.

   2. Adapt the code in part3/blink.c to (1) set GPIO pin 20 to output,
   and then in a loop repeatedly set it on and off ("clear").  
   3. Reset the pi, use the bootloader to load the code.

Hint:
  0.  When you set values in the pi registers, DO NOT OVERWRITE OLD
  VALUES!   You'll need to read, clear, and logically or in the bits
  you want to set.  If you just assign them the code for this assignment
  will in fact work, but later when you set other pins, this code will
  reset them.

 
               // assume: we want to set the bits 7,8,9 in <x> to <v> and
               // leave everything else undisturbed.
               
               x &=  ~(0b111 << 7); // clear the bits 7, 8, 9  in x
               x |= (v << 7);     // or in the new bits
                          

The code at the bottom of the page gives a simple program you can play with.

  1. You write GPFSELn register (pages 91 and 92) to set up a pin as an
  output or input. You'll have to set GPIO 20 in GPFSEL2 to output.

  2. You'll turn it off and on by writing to the GPSET0 and GPCLR0
  registers on page 95.  We write to GPSET0 to set a pin (turn it on)
  and write to GPCLR0 to clear a pin (turn it off).

Troubleshoot as before.

#### 5. Generalize your code to set/unset any pin.

Generalize your code to work with any pin from 0 to 30 (note, not all of these
are defined, but ignore that):  

   1. Note that the different GPFSELn registers handle group of 10, so you 
	can divide the pin number to compute the right GPFSEL register.
   2. You will be using this code later!   Make sure you test the code by 
	rewiring your pi to use pins in each group.

#### 6. Break and tweak stuff.

You're going to break and change your code to see effects of things going 
wrong and to make it somewhat better:

   1. Eliminate volatile from your code (easiest way: just put 

	#define volatile
  
	at the top of your file.  Recompile using "-O2".  What 
	happens?  Why?   

   2. Change your delay to increasingly smaller amounts.   What is going on?

   3. Change from using "volatile" pointer read/writes to using an 
	external function.
	The easiest way to see how:  make a simple file "foo.c" with routines
	that take in a pointer "p" and either read it by returning "*p" or 
	write it via "*p = v".  You can see the assembly gcc generates
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

#### Additional information

More links:
	1. useful baremetal information: (http://www.raspberrypi.org/forums/viewtopic.php?t=16851)
	2. more baremetalpi: (https://github.com/brianwiddas/pi-baremetal)

	3. And even more bare metal pi: (http://www.valvers.com/embedded-linux/raspberry-pi/step01-bare-metal-programming-in-cpt1)


Code to play with bitwise manipulations

             #include <stdio.h>

             void printbits(const char *msg, unsigned x) {
	             printf("%s\n", msg);
             
	             putchar('\t');
	             for(int i = 31; i >= 0; i--)
		             putchar("0123456789"[i%10]);
	             putchar('\n');
             	
	             putchar('\t');
	             for(int i = 31; i >= 0; i--)
		             putchar("01"[(x >>i) & 1]);
	             putchar('\n');
             
             }
             
             int main() {
	             // set the high and low bits to 1 for easy checking
	             unsigned x = 1 << 31 | 1;
	             unsigned v = 0b101;
             
	             // set 7,8,9 to 0b101 to show bitwise shift, mask, and, or
	             printbits("before", x);
	             x &= ~(0b111 << 7);
	             x |= v << 7;
	             printbits("after", x);
             }

