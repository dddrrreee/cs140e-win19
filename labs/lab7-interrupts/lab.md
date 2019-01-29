---
layout: page
title: Interrupts.
show_on_index: true
---

### Overview

You've probably noticed that when you have multiple peripherals, polling
between in a ``good'' way to see when something has happened
or to PWM led's difficult --- either you spend too long on
one, or you don't check it often enough.   

You can use interrupts to mitigate this problem by telling the pi to
jump to an interrupt handler (a piece of code with some special rules)
when something interesting happens.  An interrupt will interrupt your
normal code, run, finish, and jump back.  If both your regular code and
the interrupt handler read / write the same variables (which for us will
be the common case) you can have race conditions where they both overwrite
each other's values (partial solution: have one reader and one writer)
or miss updates because the compiler doesn't know about interrupt handling
and so eliminates variable reads b/c it doesn't see anyone changing them
(partial solution: mark shared variables as "volatile").

Interrupts are can be very tricky.  We are going to use them in a basic
way that hopefully mitigates some of their complexity.   

Look through the code, compile it, run it.  We'll walk through it.
