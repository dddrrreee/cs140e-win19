# The labs

### Writing boot-level code, modern techniques to validate it:

  1. [lab1-blink](lab1-blink/): get everyone up to speed and all
  necessary software installed.  You do the usual `blink` program by
  writing your own GPIO implementations based on the Broadcom document
  GPIO description.

  2. [lab2-bootloader](lab2-bootloader/): you will implement your own
  bootloader to transfer the code from your laptop to the pi.  The most
  common bootloader out there uses the `xmodem` protocol.  This approach
  is overly complicated.  You will do a more stripped down (and I believe)
  more likely to be correct protocol.

  3. [lab3-cross-check](lab3-cross-check/): you will use read-write logging
  to verify that your GPIO code is equivalent to everyone else's.   If one
  person got the code right, everyone will have it right.<br></br> A key
  part of this class is having you write all the low-level, fundamental
  code your OS will need.  The good thing about this approach is that
  there is no magic.  A bad thing is that a single mistake makes more a
  miserable quarter.  Thus, we show you modern tricks for ensuring your
  code is correct.

  4. [lab4-uart](lab4-uart/): you write your first real device driver,
  for the UART, using only the Broadcom document.  At this point, all
  key code on the pi is written by you.

  5. [lab5-replay](lab5-replay/): in a twist on lab3, you will use Unix
  system calls to interpose between your Unix and pi bootloader code,
  record all reads and writes, and test your bootloader implementation
  by replaying these back, both as seen and with systematic
  corruption.<br></br> This approach comes from the model-checking
  community, and I believe after you implement this lab and test (and fix)
  your bootloader you will be surprised if it breaks later.  (In general,
  the approach we follow here applies well to other network protocols
  which have multi-step protocols and many potential failure modes,
  difficult to test in practice.)

  6. [lab6-virtualization](lab6-virtualization/): this lab will show how
  to virtualize hardware.  We will use simple tricks to transparently flip
  how your pi code is compiled so you can run it on Unix, only shipping
  the GPIO reads and writes to a small stub on the pi.  As a result,
  you have full Unix debugging for pi code (address space protection,
  valgrind, etc) while getting complete fidelity in how the pi will behave
  (since we ship the reads and writes to it directly).

### Threads and Interrupts, with Tricks:

  7. [lab7-interrupts](lab7-interrupts/): you will walk through a simple,
  self-contained implementation of pi interrupts (for timer-interrupts),
  kicking each line until you understand what, how, why.  You will
  use these to then implement a version of `gprof` (Unix statistical
  profiler) in about 30 lines.<br></br> 
  Perhaps the thing I love most
  about this course is that because we write all the code ourselves, we
  aren't constantly fighting some large, lumbering OS that can't get out
  of its own way.  As a result, simple ideas only require simple code.
  This lab is a great example: a simple idea, about twenty minutes of
  code, an interesting result.  If we did on Unix could spend weeks or
  more fighting various corner cases and have a result that is much much
  much slower and, worse, in terms of insight.

  8. [lab8-sonar-int](lab8-sonar-int/): we take a bit of a fun break,
  and bang out a quick device driver for a simple sonar device. You
  will then get a feel for how interrupts can be used to simplify code
  structure (counter-intuitive!)  by adapting the interrupt code from
  the previous lab to make this code better.

  9. [lab9-threads](lab9-threads/): we build a simple, but functional
  threads package.  You will write the code for non-preemptive and
  preemptive context switching.  Most people don't understand such things
  so, once again, you'll leave lab knowing something many do not.

  10. [lab10-shell](lab10-shell/): You'll write a simple shell that runs 
  on Unix and can:
      + Remotely execute a set of built-in commands (`reboot`, `echo`)
      on the pi using a simple server.

      + Ship pi binaries to the pi-server, where they execute, and their
      output echoed on the Unix-side shell's console (as you did in your
      lab5's handoff test).

      + Run Unix commands locally so that, for example, `ls`, `pwd`, work.

      While it sounds like a lot, you've done much of the hard parts,
      and can just re-purpose old code (bootloader, replay, system call
      tricks) --- we're at the point in the quarter where you start to get
      some nice technological velocity because of how much you've done.


### Binary code:

TBD:

  - Implement a domain-specific, simple visual programming language
  for specifing binary formats.  You will use it to make it much
  easier to implement subsequent labs, since it greatly aids specifing
  virtual memory (page table entry, register layout), , machine code
  encoding/decoding.

  - Runtime code generation, using your language.

  - Implement a statistical version of Valgrind and Eraser using pc-sampling
  and instruction decoding.

### Virtual Memory

TBD:

  - Simple virtual memory, user-level processes.

### File system / SD Card.

TBD:

  - Implement a simple FAT32 file system using an SD card driver so you
  can read/write files to the SD card.

### Devices

TBD:
  
   - You'll implement the widely-used I2C protocol using the broadcom 
   documents

   - Implement a gyroscope / accelerometer using your protocol.

### Final projects.

Multiple options:

  - Extend your OS to do tricks in more advanced ways.

  - You'll have an expense account at sparkfun/adafruit.  You can buy
  whatever device you need and write the code to have your OS control
  it to do interesting things.

  - ? 
