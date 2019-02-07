---
layout: page
title: Sonar, interrupts.
show_on_index: true
---

### Overview

This is a fun lab.  You'll:

 1. Implement a sensor driver on your own --- a distance
 measuring device that uses sonar.    

 2. Hook the output up to an LED that gets brighter as distance gets
 shorter and struggle for a bit trying to make the variation clean.
 This is not as straightforward as it might seem, and not b/c of 
 device issues.

 3. Use the code from the last lecture to set up an interrupt handler
 to fix the problems in (2).

There's some simple skeleton code, but you're going to do all the work
yourself, including getting the datasheets and anything else you
need.  This process will foreshadow what you'll have to do when you
write drivers for devices on your own (with the simplification that
in this case we know its possible to get the needed information!).

Check-in:

 - A smoothly scaling LED that uses interrupts to smoothly dim/brighten 
 with distance.

 - Extension (easy): Build your own version of `gpio_pullup`/`gpio_pulldown`.

 - Extension (harder): have the sonar functionality use the interrupt
 rather than the LED.  This will require figuring out how to get
 interrupts from GPIO pins.  You'll have to do this on the homework,
 so why wait!


##### Motivation.

Most OS classes will have a few minutes where the words "device driver"
or "I/O devices" appear, but that's as close as many get to dealing with actual
devices and actual datasheets.  This class goes the opposite direction.
Two reasons:

 1. 90% of an OS is device driver code, so ignoring it entirely to
 spend a whole course on the other 10% of the OS doesn't make sense.
 You have to work with real devices to have informed views on how to
 package them in a larger system.

 2. After this course, the hope is that you can readily make interesting
 embedded systems (e.g., for art installations, consumer gizmos,
 gee-whiz demos for your parents when they write a tuition check).
 More than anything else, doing so requires getting through device sheets.

You could argue that being able to aggressively extract the information
you need from opaque, confusing, wrong datasheets is the main skill that
separates real system hackers from Javascript programmers.  Once you
get good at it, you'll realize what a superpower it is.

### Part 0: Understanding a breadboard

Today is the first time we'll use a breadboard.  If this is the
first time you've used one in your life, the [sparkfun tutorial](https://learn.sparkfun.com/tutorials/how-to-use-a-breadboard) is a
good, quick place to look.  Don't fry your pi.  If anything gets hot,
pull it out.

### Part 1: Implementing the HC-SR04 sonar driver.

First, find the datasheet.   Google search for 'HC-SR04 datasheet'
or some variation of that.  You're looking for the manufacturer's doc
for what to do to use the sensor.  When you find it, you'll notice it's
fairly confusing.  This is the norm.

Don't necessarily stop with the first document you find.  The first one
to show up for me is a 3-page sheet that isn't super helpful (common);
there's a longer one if you keep digging.  Also, the `sparkfun.com`
comments for the device have useful factoids (but also wrong ones,
which --- you guessed it --- is common).

The game is to skim around ignoring the vast majority of information
(which will be irrelevant to you) and find the few bits of information
you need:

  1. What the pins do (input power, output, ground).
  2. What voltage you need for power (`Vcc`).
  3. What reading you get when the the device is "open" (not signaling
	anything).
  4. what reading you get when the device is closed / signaling (`Vout`).
  5. Are there specific time delays you need to have in your code?  one
  thing about real devices: they really love ad hoc time delays, often
  around when you change their state. 

If any of the voltage values are out of range of the pi, you can't connect
directly.

Notice what the range of values are: if "low" is close to zero, but not
guaranteed to be zero, you'll have to set the input pin as a `pulldown
resistor` to clean up the signal.

Some hints, you'll need to:

  0. do the pins need to be set as `pulldown`?
  1. init the device (pay attention to time delays here)
  2. do a send (again, pay attention to any needed time delays)
  3. Measure how long it takes and compute round trip by converting
   that time to distance using the datasheet formula
  4. You can start with the pulldown/pullup functions declared 
   in `gpio.h` but then replace it
   with your own (using Broadcom pdf in the `docs/` directory).

Troubleshooting:

  0. Check wiring.   Check if the sensor is reversed.  Use specific
  wire colors to keep track of what is going where.  It's easy to be
  off-by-one.
  1. Crucial: if you lose an echo, don't get stuck in the measurement
  loop!  Retry after some maximum time.
  2. there are conflicting accounts of what value voltage you
  need for `Vcc`.
  3. Put the device so it's overhanging from your breadboard, otherwise
  you may get unfortunate reflections.

### Part 2: Use distance to control LED brightness.

Make an LED get brighter as distance decreases.

Since the r/pi does not have a way to lower voltage on its output pins,
you'll dim the LED by doing `PWM`, which mechanically means turning the
LED on/off quickly to reach a given percentage of brightness.  Write the
code to do this.  From past experience: run it on a few values without
the sonar so that you can see it works.

Now try to make it scale smoothly.  You'll likely notice some issues in
LED brightness (e.g., jagged jumps in brightness, flickering).  If you
come up with a clever hack to get around it, let us know (hint, there
is one, similar to using "cooperative threads").   However, don't spend
too much time on it --- instead adapt the interrupt code from the last
lab to brute force fix the issue by using interrupts.

##### PWM hacks.

A nice hack for PWM --- lets say we want the LED to be at 30% brightness.
So, over 10 periods, we will do 3 on's for 7 off's.  There are many
ways to arrange these on and offs.   It's easy to mess this calculation up.
Fortunately, if we change domains there's an easy solution: you can look
at this problem as drawing a line on a graphics display (i.e., where the
X and Y coordinates are integers) with a 3/7 slope (i.e., X=on, Y=off),
where we want to minimize the error of the line (so it is not jagged).
This is a well-known, old problem in graphics, which means there is some
simple, well-tested code for it. You can adapt the [Bresenham](https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C) code for it.  We
have some code in the `libpi/` directory if you want to use it.  
It makes the interrupt handler in the next part much simpler.

### Part 3: Use interrupts to PWM smoothly.

You need a way to have one job to run while another is hogging the CPU.
Make a copy of the code from last lab and do some of the work in the
interrupt handler, some in the "main thread."

You can make many different bugs in this code.  Some examples:
   - no sonar timeout
   - doing too much in interrupt handler. (what happens?)
   - no volatile (what happens?)
   - too long period for timer interrupts (what happens?)
   - not delaying long enough in the sonar readings (what happens?)
   - not disabling interrupts when getting reading (what happens?)

### Extension: Build your own version of `gpio_pullup`/`gpio_pulldown`.

These are described, with much ambiguity, in the Broadcom document on 
page 101.  Some issues:  

  1. They say to use clock delays, but do not way which clock (the pi clock?
  The GPU clock?  Some other clock?)

  2. A straight-forward reading of steps (5) and (6) imply you have
  to write to the `GPPUD` to and `GPPUDCLK` after setup to signal
  you are done setting up.  Two problems: (1) write what value? (2)
  the only values you could write to `GPPUD`, will either disable the
  pull-up/pull-down or either repeat what you did, or flip it.  

In other domains, you don't use other people's implementation to make
legal decisions about what acts are correct, but when dealing with
devices, we may not have a choice (though, in this case: what could we
do in terms of measurements?).

Two places you can often look for the pi: 

  1. Linux source.  On one hand: the code may be battle-tested, or
  written with back-channel knowledge.  On the other hand:
  it will have lots of extra Linux puke everywhere, and linux has at a tens of
  thousands of bugs at a minimum.

  2. `dwelch67` code, which tends to be simple, but does things (such
  as eliding memory barriers) that the documents explicitly say is wrong
  (and has burned me in the past).

For delays: 
[Linux]
(https://elixir.bootlin.com/linux/v4.8/source/drivers/pinctrl/bcm/pinctrl-bcm2835.c#L898) uses 150 usec.  [dwelch67]
(https://github.com/dwelch67/raspberrypi/blob/master/uart01/uart01.c)
uses something that is 2-3x 150 pi system clock cycles.  The 
the general view is
that too-much is much better than too-little, so I'd go with 150usec.  

For what to write:  from looking at both Linux and dwelch67 it *seems*
the broadcom document means to convey that after steps (1)-(4) at
set up, you then do step (6), disabling the clock, but do not do step
(5), which actually refers to the case that you are completely done with
this configuration and want to reconfigure the pin in a different way.

### Extension: Use GPIO interrupts for sonar

Rather than just put the LED pwm in your interrupt handler, you can
put the sonar functionality in there.

   1. Periodically you will trigger the sonar (say every 100ms).
   The code for this is in the interrupt handler, and just checks
   how long since it was last triggered and, if more than 100ms
   starts.  This code should be very very short.

   2. You use the pi to generate an interrupt when the echo 
   input goes from low (no signal) to hi (signal).  

Making a GPIO pin generate an interrupt is useful for many other
things in the class, so this is good functionality to figure.

First, configure the pi to:
   1. Use the pin as an `input` and as a `pulldown` (so you don't pick
   up random environmental noise from the pin, which can act as an 
   antennae).

   2. Have the pin raise an interrupt when it detects a rising edge.

   3. As with the timer, we also have to enable general interrupts,
   so configure the general interrupts (chapter 7) to be tied to 
   the GPIO pin you picked.  The IRQ table on page 113 is kind of 
   confusing, so you'll have to think about what it probably means
   (hint: 49).

   4. In the interrupt handler: you'll have to figure out if your
   pin caused the interrupt (`EVENT_DETECT`) and, then, clear it.
   
Test this code by running power through an LED (to bring down the current)
and touching your input wire to the LED leg.   If you print out in the
interrupt handler when it triggers, this should match when you touch.
Singe speed of light is much faster than people, you may get multiple
prints for a single touch.

Now restructure your sonar code to use this interrupt to detect when
an pulse has returned and caused echo to go high.  This is actually the
easiest part.
