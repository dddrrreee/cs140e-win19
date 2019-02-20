## Exporting your pi as a file system.

Today you're going to take your pi shell-system and use the FUSE file
system to mount it as a normal file system on your laptop.    Each ability
of the pi will be exposed to the user by mapping it to its own file and
potentially overriding the sematics of `read()` and `write()` with custom
actions.  

For example:

 - `/pi/reboot`: if you write anything to this file, it will reboot the 
 pi.

 - `/pi/echo`: if you write anything to this file, it will be echoed
 by the pi.

 - `/pi/console`: contains all output from the pi.  

 - `pi/run`: writing a pi binary to this file will cause the pi to run
 it (and the output appended to `pi/console`).

This approach lets you control your pi using standard Unix utilities
rather than writing a custom shell to export different actions.  E.g.,
running `tail -f /pi/console` will cause all output from the pi to 
be echoed to your terminal.

#### The big picture

In the previous lab, you wrote a trivial shell that would look at what
the user typed and decide if it was:

  0. A built-in command: run on the pi.
  2. A pi program: run on the pi.
  1. Anything else?  Assume its a unix command: run on your laptop.

Building it was a good proof-of-concept for seeing one way to meld
your usual computing environment with your pi system.  On the other
hand, it's clear it's a dead-end, at least without significant work.
Among many reasons: (1) it is, and will remain, much less full-featured
than your regular shell (no auto-completion, no history, no job control,
no environment variables, etc) , (2) we are far from running any Unix
code on the pi.

In some sense, what we'd like is some way to extend your current shell
(`bash`, `sh`, `tcsh`) so that it does all its normal laptop-stuff,
but calls out to the pi for pi-stuff.  This would likely require having
each different shell have some extensibility method and require us to
write a bunch of uninteresting shell-specific code.

We're going to side-step the problem to make it easier and our solution
better.  One surprise in systems is that it can sometimes be easier
to solve a more general problem than the one you are focused on.
This situation is a good example of that.

Rather than building pi-specific knowledge into the shell, we instead
extend the pi as a file system, which your shell can then work with as
it does any other sets of directories, files, links.

Your shell is already good at dealing with file systems, and your laptop
has lots of programs to do so.  Once we wrap the pi up as a set of files,
directories, links, then you can do everything you wrote in your Unix-side
shell and much much more, while cleanly causing any pi-action to occur.

You now have autocompletion, history, job control, etc without having
to write any code.

Most OO programming --- at best --- allows you to wrap different nouns up
with the same verbs, so that a single piece of code that works on those
verbs can work on any specific instance.  If I could pick one single
success story of this approach, and the one that it simultaneously the
most under-appreciated, it would be the Unix file system interface.

If you can map your thing (e.g., hardware device) to you immediately
have it integrate, fairly cleanly, with all the existing code out there.

Of course, are plenty of things that do not fit the Unix model of
`open()-read()-write()-close()`.  A simple way to get thousands of
examples: `grep ioctl` in Linux device directories and look at all the ---
oft-buggy --- code that was stuffed into these `ioctl`-routines because
it did not fit nicely within the Unix file interface.


### Check-off

You need to show that:

  1. Your pi-FS works as a standard file system.  `make part0` wil
  run some simple tests.  You should be able to do file operations 
  such as:

    % echo "hello world." > /pi/echo
    % cat /pi/echo
    hello world.
    %

  Have them work as expected, and have all output appear in `/pi/console`.

   2. In order to hook up your pi-FS to you pi-sh: write Unix code that
   can fork/exec a program, overriding its file descriptors for
   `stdin` (0), `stdout` (1), and `stderr` (2) so that it can interact
   with the program identically as a user typing from the console.

   3. Hook up your pi-FS to your pi-sh: when the user writes values
   to `/pi/echo`, `/pi/reboot`, `/pi/run` the relevant command is
   sent to the pi (via the shell) and all shell-output is written to 
   `/pi/console`.

Extensions:

   1. Add directories.
   2. Add `/dev/led` so that writes turn the led on/off and that 
   reads give the current state.
   3. Add a `pthread` thread to your FS that will pull output from
   the pi in the background.

----------------------------------------------------------------------
## Part 0:  Install and use FUSE (15 minutes)

Make sure you have FUSE installed.  


##### MacOS

From the FAQ (https://github.com/osxfuse/osxfuse/wiki/FAQ):

  2.1. What is the recommended way to install "FUSE for OS X"?

  The recommended way to install "FUSE for OS X" is to download the latest
  "OSXFUSE-{version}.dmg" available from http://osxfuse.github.com/
  and double-clicking on "Install OSXFUSE {version}".



##### MacOS

From the FUSE `github` repo (https://github.com/libfuse/libfuse):

    apt-get install gcc fuse libfuse-dev make cmake

##### What to do:

Run the `hello` example in `lab11-fuse-fs/part0-hello`.  It's
taken from (https://github.com/libfuse/libfuse/tree/master/example).
There are a bunch of other examples there.  Change the code so
that it prints out:

    hello cs140e: today let us do a short lab!

##### Background reading:

Some other places to read:

  1. A reasonable [1-page rundown](https://engineering.facile.it/blog/eng/write-filesystem-fuse/).

  2.  A longer, [old-school rundown](https://www.cs.nmsu.edu/~pfeiffer/fuse-tutorial/html/index.html), with code.


----------------------------------------------------------------------
## Part 1: Implement FUSE methods to make a simple FS (45 minutes)

If you look in `part1/simple-fs`:

   - There is starter code to implement a simple FUSE file system.  
   - Typing `make test` will simple Unix commands to check if your 
   system is broken.

You'll have to implement five methods:

   - `getattr` get attributes of the file: its size, permissions, and
   number of hardlinks to it.

   - `read`: read fro the file at a `offset` of `size` bytes.

   - `write`: write to the file at a `offset` of `size` bytes.

   - `readdir`: return the directory entries in the given directory.

   - `ftruncate`: truncate the file.

Other than `getattr` (which we already defined), these are more-or-less
identical to Unix calls, so you can just man page them to figure stuff
out.  If you run FUSE with debugging output (which we do) you can see
where it is failing and why (usually permission errors).
