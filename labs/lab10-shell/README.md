## Writing a trivial shell. 

By the end of today, you'll put together several pieces of code you've already
built so that you have a trivial shell that runs on Unix and can:

   1. Remotely execute a set of built-in commands (`reboot`, `echo`) 
   on the pi using a simple server.

   2. Ship pi binaries to the server, where they execute, and their output
   echoed to the screen (as you did in your handoff test).
 
   3. Run Unix commands locally so that, for example, `ls`, `pwd`, work.

While it sounds like a lot, you've done much of the hard parts, and can just
re-purpose old code.

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
   runtime of programs (to catch when they infinite loop).
   4. Add the ability to run shell scripts (not hard, but fun).

----------------------------------------------------------------------
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

For the Unix-side you'll modify `pi-shell.c`, we gave you some useful 
helpers in `shell-support.c`.

----------------------------------------------------------------------
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

----------------------------------------------------------------------
## Part 3:  Running remote programs (1 hour)

This final piece involves you extending your shell so that the Unix-side
can send a single program at a time (hereafter: `hello.bin`) to the
pi-side, which will run the code, and then tell the Unix side when it has
finished. Before termination, the Unix-side simply echos all characters
it receives from the UART to standard output.

At a high level you will re-purpose your bootloader code both (the Unix
and pi side) so that it can send programs from within the shell.

We first talk about some of the issues and then what you need to do 
concretely.

For the pi-side you'll modify `bootloader.c` and `pi-shell.c`.

#### Running multiple programs: Some Issues

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

  5. What if `hello.bin` infinite loops?

  6. How do we run multiple programs?

  7. How can `hello.bin` call the main program?  For example, if it
  creates threads internally and does a `yield()` this will not be able
  to call the main scheduler.  We might or might no like this behavior,
  depending on context.  Similarly, for `kmalloc`, where each program
  will have its own private heap.  (Sometimes nice for isolation, sometimes
  not so nice because of fragmentation.)

There are more-or-less the problems we'd have to solve to make a full-on
multi-user operating system.  Since we want to get you up and running in
a couple of hours, we solve most of these issues either through gross
hacks or simply pretending they don't exit.  The rest of the quarter
will involve (among other things) eliminating these hacks and solving
the associated problems to make the scheme less embarrassing.

#### Some hacks and non-solutions:

For today:

  1. We statically link the code at a "known" address so that when we
  ship it over, your bootloader can simply copy it to that address and
  jump to it, without needing to fix up any absolute addresses the code
  references. (Note that relative addresses "just work" since they
  do not depend on the code's location.)

  2. When the program `hello.bin` finishes on the pi, the pi-side sends
  an agreed-upon string to the Unix-side (`"CMD-DONE\n"`) so that it
  will know to stop echoing output.<br></br>
  Without some form of job-control the Unix-side has no insight into
  if the process finished or not or --- at the most concrete level
  possible --- whether to print a `>` character, signalling it can run
  a new command, or to wait to see if there's another character coming
  that it will need to echo.

  3. If `hello.bin` crashes, we are out of luck.  We will be doing
  virtual memory fairly soon, which gives us a way to attack this problem.

  4. We manually rewrite the `hello.bin` code to eliminate `reboot()`
  (which we have so far used as an `exit()`), `uart_init()` (which
  will have already occurred), and change `start.s` to not setup a stack
  pointer internally, but instead just assume that it already has one.

  5. inf-loop: you can actually solve this easily (see extension) even
  with non-preemptive threads.

  6. Multiple programs: just use your threads!  You will need some way
  to track the address ranges that are in use, and some way to know when
  a given thread is finished.  Both require less than 10-20 lines of code.

  7. Calling code in the main program (the pi-side shell code): We
  ignore this problem for the moment, but will get to it fairly soon.
  The intuition is that its not hard to do so by using dynamic linking or
  using system calls (implemented using the `SWI` exception on the ARM).

#### What you will do

##### A. What we do to binaries

To summarize the above:

 1. We statically link each binary to a free address.  The legal free
 range is defined in `shell-pi-side/pi-shell.h` --- it is all addressees
 above the highest address we have ever used in the code we've written
 so far and below what the end of the pi's physical memory is.

 2. We only run one program at a time so don't have to keep track of
 what is free or not in any kind of fancy way, nor which input/output
 is for/from which program.

 3. We rewrite the code to avoid any ugly problems such as trashing
 the shell's stack, reboots, or re-initializing the UART.  

You can see these concrete changes by examining the code in
`lab10-shell/hello-fixed` directory.  You should be able to describe
what the changes are compared to a normal `hello` and why.

##### B. What we do to send binaries

At this point, I was hoping to triumphantly say you would ship programs to
the pi by simply using your bootloading code as-is, thereby vindicating my
foresight and the generality of what you've done already.  Unfortunately,
while you *can* use your code with minimal modifications, I think it's
probably much better to strip the code down to something simpler.
The result will be easier to debug and get around some of the race
conditions in the original protocol, which arise when the protocol's
insufficient flow-control hits the UART's 8-byte finite receive
queue size.

While you won't be using the code as-is, the understanding you gained
from writing it the first time should allow you to create a custom
protocol pretty quickly.  Hopefully.


This is a stripped down version (explained more below):

        =======================================================
         unix side                          pi side 
        -------------------------------------------------------
                                         
                                            put_uint(ACK);

        expect(fd, ACK);


        // version: stored by linker
        put_uint(fd, code[0]);
        // address: stored by linker.
        put_uint(fd, code[1]);
        put_uint(fd, nbytes);
        put_uint(fd, crc32(code, nbytes));
   
                                           <sanity checks>
                                           put_uint(ACK);

        expect(fd, ACK);
        <send code>
        put_uint(fd, EOT);

					   <get code>
                                           expect(EOT);
                                           <check code crc>
                                           put_uint(ACK);

        expect(fd, ACK);

                                           <done!>
        =======================================================


More descriptively:

 1. The Unix-side shell code sends the pi-side an ASCII command (e.g.,
 "run <program name>").   You will do this even if you use your old code.

 2. The pi side prints this (so you can double-check) and then waits for
 an `ACK`, forcing the Unix-side to wait until its ready.

 3. The unix side sends: the version, the address the code is linked at,
 its size, and a CRC of the code (`crc32(code, nbytes))`).  The linker
 script is written to that it stores the version as the first word of
 the binary, and its linked-to address as the second.  We use a version
 so that you know what version of the boot protocol you are using and
 can extend it later --- for us `version=2`.

 4. The pi-side checks the address and the size, and if OK, sends  an
 `ACK`.  Otherwise it does a `put_uint` of the right error message
 (sending different conditions will help debug, since you can print them
 on the Unix-side).  **NOTE: the pi-side code cannot print at this point
 since the Unix-side is expecting raw bytes.  Doing so makes your code
 not work.**

 5. The Unix-side sends the code and an `EOT` and then waits for an `ACK`.

 6. The pi-side copies the code to `addr` (as before), checks the
 checksum, and if its OK, sends an `ACK` and then jumps to `addr`.

The use of `ACKS` prevents the Unix-side from overrunning our finite-sized
queue.  The range checks and the checksums guard against corruption.

We are sleazily running the code on our stack, so after you bootload,
you can simply jump to it.

#### Done!

At this point you will have a very simple shell!  A full-featured one
is a lot more code, but not alot more ideas.

----------------------------------------------------------------------
## Part 4:  Extensions.

There's several things you can do.

 0. Add more useful commands to your shell.   I assume you've been annoyed
 that there is no `cd`!  (Which is not a Unix command, but rather is
 built-in to the shell.)

 1. Extend the shell to be able to run on scripts (e.g., files that
 have `#/usr/bin/pix` as their first line).   

 2. Run the program in its separate thread so the pi-side shell stack is
 not corrupted.  You can then extend your Unix side with a `&` operator
 so you can run multiple programs.  Because we are running cooperatively,
 we might actually be able to make good progress without trashing shared
 state (such as the UART).

 3. Add a `rpi_alarm(code, ms)` function (using timer-interrupts) to your
 threading package so that it will `code` when an alarm expires.  You can
 use this to make a "watch-dog timer that kills a too-long running thread.
 You will have to also add `rpi_kill(tid)`.  Alternatively, you could
 add pre-emption to your thread code, and simply have your pi-code run
 in a loop, yielding control until too-much time has passed, upon which
 it kills the running program.

 4. Compile some other programs using fixed addresses (e.g., `sonar`)
 and change the delay code to `rpi_yield()` rather than busy wait..
