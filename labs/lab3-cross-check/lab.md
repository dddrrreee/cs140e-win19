---
layout: page
title: Quick overview.
show_on_index: true
---

Lab: automatically cross-check your gpio against everyone else's.
======================================================================

A goal of this course is that you will write every single line of
(interesting) low level code you use.  A good result of this approach is
that you will understand everything and, unlike most OS courses, there
will not be a lot of opaque, magical code that you have no insight into,
other than sense of unease that it does important stuff beyond your ken.

An obvious potential result of this strategy is that since you
are writing all code your system depends on, a single bug can
make the rest of the quarter miserable.  

This lab will have you build the inital pieces needed to automatically
check that your homework code is correct.

After completing the lab you will:

	1. Be able to check that your `gpio` code is equivalant to
	everyone else's in the class by tracing all reads and writes
	it does and comparing them to everyone else's implementation.
	If even one person gets it right, then showing equivalance means
	you got it right too.  And, nicely, automatically detect if any
	subsequent modifications you do break the code.

	2. Be able to quickly, correctly write a more full-featured
	`gpio` implementation by checking its equivalance against the
	fairly complicated code used in the initial cs107e class.

After this lab you can then most of the rest of the homework without too
much issue.

#### 1. Intercept all GPIO read and writes (10 minutes)

For this lab and the homework, we need to be able to override what
happens when your code writes to GPIO memory.  Later in this course
(when we build virtual memory), you will use memory protection tricks to
intercept all loads and stores and cause them to do interesting things.
For the moment, we will do things in a much simpler, though manual way.

	0.  Do a pull.	 Your `lab1/part3`  should now have an `rpi.h`
	with definitions for `get32` and `put32`.

	1. Rewrite your `blink` code from lab1/part3 to use `get32` and `put32`
	instead of a raw read or write of device memory.  So, for example

	     	*gpio_fsel0 &= ~111b;

	would become:

		put32(gpio_fsel0, get32(gpio_fsel0) & ~111b);


	2. Run this code on the pi to make sure it works.  If it doesn't
	work, fix it, or the rest of the lab is going to awful.  

	3. To make compile-edit-debug faster, use the old bootloader we
	gave you in lab1 to ship the code to the pi (make sure to put
	it back on your SD card!).  Or, if you are far along, your new
	one that you just wrote!

#### 2. Make a simple Unix testing harness (30 minutes).

In order to make testing easy, we want to be able to run your r/pi code ---
unaltered --- on your Unix laptop.  What do you need to do so?  

In general:
	
	1.  You'll need to do any setup and then call `notmain`.

	2. You'll have to write fake versions of any pi-specific routines
	your code calls.  Since your `blink` is simple, all you'll have
	to do is provide fake implementations of `get32` and `put32`.
	Also, since we can't do arm assembly code, you'll have to 
	provide an empty definition for `delay` (for the moment, you
	can just comment out the body of your version).

For the moment, you'll implement `get32` and `put32` by making a simple,
tracing memory (an array that prints on each read or write) that on on
reads of any address `addr` returns the last value written to it (just
like normal memory) or, if no value was written, initializes the memory
associated with `addr` to a random value and then returns that.

On `put32(addr,v)`: 

	1. Create an entry for `addr` in `mem` if it doesn't exist.

	2. Write `v` to `addrs` entry.

	3. Call `print_write(addr,v)`.

On `get32(addr)`:

	1. If `addr` does not exist, insert `(addr, random())` but do
	not print anything.

	2. Get the value `v` associated with `addr`.

	3. Call `print_read(addr,v)`.

	4. Return `v`.


To test it:

	1. Take your `blink.c` code from lab3 and put it in
	`simple-cross-check/blink.c` here.

	2. Compile.

	3. Run `./test-blink` on Unix.  It should run without crashing and,
	importantly, print out the values for each `put32` and `get32` in the
	exact order they happened.  

	4. If you pipe the result through `cksum`,
	it should give you the same value as your lab partner has.

	`./blink.unix | cksum`

If these values match, you know your code worked the same as your partner's.

#### 3. Check your code against everyone else (5 minutes)

We want to check that your `gpio` code works the same as everyone
else.  Given the `get32` and `put32` modifications above, a 
simple, stringent approach is to check that:

	1. It reads and writes the same addresses in the same order 
	with the same values.

	2. Returns the same result.  

If so, then we know that --- at least for this input --- they have the
same effect.

Thus, if we run them on all "interesting" inputs, we can exhaust their
behavior and compare.

For this section:

	1. Test your `gpio_set_output`, `gpio_set_on` and `gpio_set_off`
	using the full generator version (which runs them on many values)
	individually against your partner.   

	2. Post the cksum value for each to the newsgroup.

	3. By the end of the class everyone should have the same result.


#### 4. Sign-off

	1. Your original `gpio_set_output`, `gpio_set_on`,
	`gpio_set_off` give the same result as other peoples.  You can
	run them in this order and just hash the end result.

	2. Show that it gets the same value as the original.

#### 5. Extra credit: Reimplement `gpio.o`

We gave you a complicated implementation of gpio
(`cs107e-complex-gpio/gpio.c`).  Check it's verion of `gpio_set_output`
against yours.  Implement your own `gpio_set_func` and annotate each
interesting thing with the page number and any sentence fragment from
broadcom document.   Verify that your code is equivalant.  You'll have
to modify the given gpio.c code since it does not do error checking and
you should.

#### 6. Advanced.

Making the code more useful mostly involves expanding it so that 
even when code writes values in different orders, you can still
show equivalance.

Modify your code to:

	1. Check that your opposing blink code is identical to your 
	partners.  What should you do about `delay_ms`?

	2.  Show that its equivalant even when you set output pins
	in different order.  Hint: you likely want to be able 
	easily mark some memory as "last writer wins" where it doesn't
	matter the actual order, just the final value.   I would 
	do this by tracing each function, automatically marking the 
	addresses it writes as last-writer addresses.

	3. How to show equivalance when you set and clear in different
	orders?  This is trickier.  You will need to come up with a clean
	scheme to indicate that non-overlapping bit-writes do not 
	interfere.

	4. Within a device, often you can write some fields in 
	any order, and there is a final "commit" location you write to
	turn the device on.  Devise a good way to cleanly indicate these
	differences.


	5. Strictly speaking, we need memory barriers when writing to
	different devices.  Add support for checking this.

If you can do all of these you are in great shape to check some
interesting code.

Cross-checking Background
---------------------------------------------------------------------

One way to compare two pieces of code A and B is to do so by just
comparing the code itself.    This is what you've been doing so far
in class: when your code doesn't work, you'll sometimes stare at 
someone else; or when it does, you'll stare to verify you both did 
the same.  This can be a fine approach.

If two pieces of code are identical, then we can trivially do this
automatically using something like `diff` or `strcmp`.  However, if the
code differs in any way besides mere formatting, then a simple string
comparison fails.

The other approach is to just run tests, and compare the test output.
A nice result of this approach is that even if two pieces of code
look wildly different we can still automatically detect when they are
equivalant by comparing their outputs.  This is why your classes use
it for homeworks.

The better your testing, the more chance you have of finding where
the code differs.  The intuition here is that we don't actually care
how the code works, just that it has the same end result --- i.e., its
"side-effects" are the same.  In general, side-effects include anything
code does visible to the outside world: its writes to memory, what it
prints, any network packets it sends, etc.

We're going to do a special case version of this check, since for the r/pi
code so far, all we care about are reads and writes to device memory.
If we record these, and compare them to another implementation, if
they read or write the same memory in the same order, we know they are
the same.  (Later, when we get more advanced, in some cases they can
read or write in different orders, but we ignore this for now.)

A drawback of this approach that we have glossed over, is that we
only check equivalance on the inputs we run them on, so to really show
equvalance we need to exhaust these.  Fortunately, if you look at the
r/pi code we have written so far, it has simple inputs --- either none
(where it just reads/writes device memory to initialize it) or a single
pin input, which we can more-or-less exhaustively test (`0..31` and some
illegal values).
