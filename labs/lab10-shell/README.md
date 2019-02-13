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

   1. `hello-fixed/`: simple test hello-world program that can be linked
   at different addresses (change in the `Makefile`).

   2. `shell-unix-side/`: the Unix side code for the shell.  It has a number
   of built-in commands (`reboot`, `echo`), will ship binaries to the pi for
   files that end in `.bin`, and will fork-exec everything else on the Unix 
   side.  (So, for example, `ls`, `pwd`, work).

   3. `shell-pi-side/`: the pi side code for the shell.  It loops, reading
   newline-terminated lines from the UART, and acts on the commands or 
   gives an error message.

### Check-off

You need to show that:

   1. Your shell can run Unix commands locally and pipe their output to 
   standard out.  

   2. That the built-in commands `reboot` and `echo` work.

   3. That you can remotely run programs.

Extensions:

   1. Run the program in a thread.
   2. Add `&` to your commands so you can run multiple programs at once.
   3. Add a `set timeout <secs>` command so that you can limit the 
   runtime of programs (to catch when they inf loop).
   4. Add the ability to run shell scripts (not hard, but fun).


## Part 0:  Getting handoff and to work (15 minutes)

This step is just getting your handoff code to work with the shell.
If you type `make run` in the `shell-unix-side` directory, it will
remind you how to invoke your `my-install` so that it hands off the pi
file descriptor to a subprocess:

      my-install -exec ./pi-shell ../shell-pi-side/pi-shell.bin


As a simple test, just have `shell-pi-side` code echo back each character
it receives.

## Part 1:  Add some built-in commands (20 minutes)

Add logic to both the Unix and pi side so that they recognize two commands:

   1. `reboot`: sends a reboot message to the pi, and waits for it to 
   send back a "PI REBOOT!!!\n" message.  The pi should send the message,
   delay for some time (e.g., 100ms) so the UART can flush, and then 
   reboot.

   2. `echo ...`: sends the entire line to the pi, which simply sends it back.

   3. `<name>.bin`: this simply prints out that the program is a pi-program
   and does nothing else (you'll add this in Part 3).

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
  error you get from `execvp` since that means the command the user 
  typed was invalid and you should print a message before going to the
  next one.

  3. In the parent: wait, and print out the exit status of the 
  child if it failed.

This code can be taken directly from your homework.

You can add support for error and output redirection.
The CS140 website has example code for 
[doing so](http://www.scs.stanford.edu/19wi-cs140/notes/) --- `redirsh.c`
and the rest.

## Part 3:  Running remote programs (1 hour)
