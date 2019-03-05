## Projects and a simple watch/execute program

Today is a light lab.    
 - First, we will write a short 50-line program that will hopefully help you 
   a lot in your later CS work.  

 - Then the rest / most of the lab will be you kick-starting your final
   project by deciding on a topic, starting to write some code.  I brought
   a bunch of devices and boards with me so you can save on USPS time
   if they seem like they could help you do something.

#### cmd-watch

IMO `sublime` and other IDE's have  crippled how people code --- after
saving a file, often there's 30-60seconds of switching screens, mouse
clicks, etc before you actually get a compile and edit done.  I'd assert
that you're more likely to stay in a flow state as soon as you save code, 
all the compile-run happens automatically and immediately.  

So we are going to build a simple tool that that lets you do so.  It works as follows.
  1. You invoke it:

          cmd-watch ./program-name arg1 arg2 

  2. `cmd-watch` will monitor all the `.c`, `.h`, `.S`, `.s` files in the current
     directory and if they are modified, execute the command `./program-name` with
     the arguments `arg1`, `arg2`, etc.
  3. For example:

          cmd-watch make
     will run `make` if any of the files `*.[chsS]` change in the current directory.

This means you can be editing any file, save it, and then --- without
moving a finger --- the code will be compiled and (depending on your
makefile) run.  IMO, the best coders have everything they do either
require one keystroke or (better) zero.  This is a good step in that
direction.

You can implement `cmd-watch` as follows:
  1. Uses `opendir` to open the current directory and scan all 
  entries using `readdir`.
  2. For any entry that has the suffix we are looking for, uses `stat` 
  (`man 2 stat`) to get the `stat` structure associated with that entry and check
  the modification time (`s.st_mtime`).
  3. If the modification time is more recent than the last change in the directory, 
  do a `fork-execvp` of the command-line arguments passed to `cmd-watch`.
  4. Have the parent process use `waitpid` to wait for the child to finish and also
  get its status.  The parent will print if the child exited with an error.

Some gotcha's
  - If nothing changed, go to sleep for a human-scale amount,  otherwise
  you'll burn up a lot of CPU time waiting for some entry to change.  The claim
  is that people can't notice below 100ms, so you could sleep for 250ms
  without too much trouble.

  - Make sure you close the directory (`closedir`) otherwise you'll run
  out of file descriptors.

  - Even if you find a recent-enough modification time to kick off the
  command, scan the rest of the directory.  Otherwise you can kick off
  spurious executions.

#### Writing the code (should take 30-40 minutes, hopefully)

In terms of code, you're on your own.   At this point you should be able to 
quickly put together different pieces from old labs:
  1. Copy one of the more recent Unix `Makefiles` from another lab.
  2. Make a `cmd-watch.c` file with a main that takes `argc`, `argv`.
  3. Grab your `fork-execvp-waitpid` code from the bootloader.
  4. Start hacking!

For people on MacOS:
  - To make this more immediately useful, I would strongly, strongly
  urge you to change your Unix side bootloader to work-around MacOS's
  serial device slowness by having the bootloader code retry opening the
  serial device until it actually shows up.  That way you won't have to
  keep manually retrying yourself.  Probably sleep for 1 second between
  attempts, give up after 10.

#### Checkoff
  
Show that you can do `cmd-watch make` in a lab directory and show that it
will make the code and kick off a pi-boot-loading sequence.  You might
have to modify the lab's makefile to do a sleep in the right place if
your bootloader needs it.

There are a bunch of extensions you can do:
 - Make it easy to use different suffixes or scan in different locations.

 - Run multiple commands, for example `foo \; bar` which will run `foo` and then
  run `bar`.  Note if you use a semi-colon as a separator you will likely need to 
  escape it or your shell will interpret it.
