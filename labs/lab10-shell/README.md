## Writing a trivial shell.

By the end of today, you'll put together several pieces of code you've already
built so that you have a trivial shell that runs on Unix and can:

   1. Remotely execute a set of built-in commands (`reboot`, `echo`) 
   on the pi using a simple server.

   2. Ship pi binaries to the server, where they execute, and their output
   echoed to the screen (as you did in your handoff test).
 
   3. Run Unix commands locally so that, for example, `ls`, `pwd`, work.

While it sounds lke a lot, you've done much of the hard parts, and can just
repurpose old code.

### STOP: do this first!

Before you do anything --- make a symbolic link to your homework 1 bootloader
directory:

    > cd lab10-shell
    > ln -s <path>/cs140e/homeworks/1-bootloader/bootloader bootloader
    > ls bootloader
          echo  Makefile  pi-side  replay  shared-code ...
    > make

All the directories should make fine.

#### What's in the directories:

   1. `shell-unix-side/`: runs on the Unix side, and orchestrates all
   actions on behalf of the user.  It loops, reading a line at a time
   from standard input, parsing this, and then acting on the result.

   3. `shell-pi-side/`: the pi side code for the shell.  It loops,
   reading newline-terminated lines from the UART, and acts on the
   commands or gives an error message.

   3. `hello-fixed/`: simple test hello-world program that can be
   linked at different addresses (change in the `Makefile`), used to
   test remote execution.

### Check-off

You need to show that:

   1. Your shell can run Unix commands locally and pipe their output to 
   standard out.  

   2. That the built-in commands `reboot` and `echo` work.  You catch
   control-C and send a reboot to the pi.

   3. That you can remotely run programs.

Extensions:

   1. Run the program in a thread.
   2. Add `&` to your commands so you can run multiple programs at once.
   3. Add a `set timeout <secs>` command so that you can limit the 
   runtime of programs (to catch when they inf loop).
   4. Add the ability to run shell scripts (not hard, but fun).

## Part 0:  Getting handoff to work (15 minutes)

This step is to just hand-off the pi file descriptor to the Unix-side
shell program.  You've already done this in your `handoff` code for
lab5. 

If you type `make run` in the `shell-unix-side` directory, it will
remind you how to invoke your `my-install` so that it hands off the pi
file descriptor to a subprocess:

      my-install -exec ./pi-shell ../shell-pi-side/pi-shell.bin

As a simple test, just run the `hello.bin` from the UART lab, and 
echo the results to the terminal:

      my-install -exec ./pi-shell ../lab4-uart/uart-tests/2hello-once.bin

## Part 1:  Add some built-in commands (20 minutes)

Add logic to both the Unix and pi side so that they recognize two commands:

   1. `reboot`: sends a reboot message to the pi, and waits for it to 
   send back a "PI REBOOT!!!\n" message.  The pi should send the message,
   delay for some time (e.g., 100ms) so the UART can flush, and then 
   reboot.

   2. `echo ...`: sends the entire line to the pi, which simply sends it back.

   3. `<name>.bin`: this simply prints out that the program is a pi-program
   and does nothing else (you'll add this in Part 3).

   4. Use `sigaction` ("man sigaction") to catch control-C, and send
   a reboot command to the pi.  Make sure the variable you set in the
   signal handler is `volatile` or the compile may not realize that it
   can change without an explicit write.

So, for example, if you type "make run" in `shell-unix-side` you'll get
something like this (any `PIX:>` prompt has the command I typed):

     % make run
     my-install -exec ./pi-shell ../shell-pi-side/pi-shell.bin
     FOUND: </dev/ttyUSB0>
     about to exec <./pi-shell>
     PIX:> echo foo
     PIX:builtin cmd: echo
     PIX:pi echoed: <echo foo>
     PIX:> echo fadfa fda fda fdsaf fd
     PIX:builtin cmd: echo
     PIX:pi echoed: <echo fadfa fda fda fdsaf fd>
     PIX:> reboot
     PIX:have a <reboot>
     PI REBOOT!!!
     PIX:pi rebooted.  shell done.
     my-install: exited with: 0
     my-install: Done!

This is obviously very chatty.  When your code works, you'll likely cut down
on the verbose.

We provide some routines to make this process easier, but you can of course
write your own.

## Part 2:  Getting local commands to work (20 minutes)

If the user typed a string that is not a builtin command, you should
attempt to execute it locally:

  1. Use `tokenize` to split the line up into multiple tokens.

  2. Attempt to fork-exec using these tokens.  Make sure to print any
  error you get from `execvp` since that means the command the user typed
  was invalid and you should print a message before going to the next one.

  3. In the parent: wait, and print out the exit status of the child if
  it failed.

This code can be taken directly from your homework.

You can add support for error and output redirection.
The CS140 website has example code for 
[doing so](http://www.scs.stanford.edu/19wi-cs140/notes/) --- `redirsh.c`
and the rest.

## Part 3:  Running remote programs (1 hour)

Conceptually, running a program on the pi through the shell is pretty simple:
    1. Just copy it over (you already have bootloader code to do so).
    2. Jump to it.
    3. Profit.

Unfortunately, there's a bunch of problems we need to solve for this.  
A subset:

    1. We have to run the code somewhere other than where the `pi-shell.bin`
    code is running.  How do we get it to run there if it was linked elsewhere,
    how do we keep track of which memory is free?

    2. We need to tell the Unix side when the code is done so that it can
    stop echoing the output.

    3. What if `hello.bin` crashes?

    4. What if `hello.bin` calls `reboot()` or sets its stack pointer to 
    ours?

    4. What if `hello.bin` wants to call our code?

    5. What if `hello.bin` infinite loops?

    6. How do we run multiple programs?

    7. How can `hello.bin` call the main program?

Since we want to get you up and running in a couple of hours, we solve
most of these issues either through gross hacks or simply ignoring them.
The rest of the quarter will involve (among other things) eliminating
these hacks and solving the associated problems to make the scheme
less embarassing.

For today:

  1. We statically link the code at a "known" address so that when
  we ship it over, your bootloader can simply copy it to that address
  without needing to fix up any absolute addresses your code references.

  2. When the program `hello.bin` finishes on the pi, we echo back an
  agreed-upon string so that the Unix end knows to stop echoing output.
  (Otherwise it does not know if there's another character coming.)

  3. If `hello.bin` crashes, we are out of luck.

  4. We rewrite the `hello` code to eliminate reboot (which we have 
  so far used as an `exit()`) and change `start.s` to not setup
  a stack pointer internally but instead just assume that it has one.

  5. inf-loop: you can actaully solve this easily (see extension).

  6. Multiple programs: just use your threads!  You will need some way
  to know when a given thread is finished, but you can write the code
  as a client, or extend your threads package.

  7. We ignore this problem; but its not hard to add by doing a form
  of dynamic linking (or using system calls).

So, to summarize: 

 1. We statically link each binary to a free address (the free range is
 defined in `shell-pi-side/pi-shell.h` and is just anything above the
 highest address we currently  use).

 2. We only run one program at a time so don't have to keep track of
 what is free or not in any kind of fancy way, nor which input/output
 is for/from which program.

 3. We rewrite the code to avoid any ugly problems such as trashing the
 shell's stack, reboots, or re-initializing the UART.

