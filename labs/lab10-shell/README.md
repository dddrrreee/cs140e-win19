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

