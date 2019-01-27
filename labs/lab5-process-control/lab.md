## Lab: testing your bootloader using process control and fault-injection.

This lab will show how to use Unix verbs (`fork`, `exec`, `waitpid`)
and nouns (sockets, pipes, processes) to control processes and make them
do tricks.

Our main trick will be testing your bootloader code by using another
process to intercept all its communication and cause it to fail in a
variety of ways, making sure it does not crash.

Since man pages are not always clear, `useful-examples` has a bunch
of concrete examples showing how to use the system calls you'll need.
You should go through and read/run/modify the various programs to get
a reasonable feel for how things work.

### Don't make these mistakes.

The single largest mistake I see people do over and over again in lab is not
checking if a system call failed.  Given that this is many people's
first experience with these calls, the probability that they are used
incorrectly is 1.  Further, given that we are using hardware, the chance
that a call fails because of a pi issue is 2.  Checking calls will
save you a huge amount of time, since otherwise you have to invert the
function "my program doesn't work" which is wildly non-biijective.
You can do something as simple as:

    if(write(fd, buf, n) < 0)
      sys_die(write, write failed?)



A minor technical mistake: if you are using `printf` to see what's
going on, MacOS will often buffer the output,  and not print til later.
This can be confusing since you will then believe code has not run when
in fact it has.  To get around this either:

	1.  Put a newline on any `printf` (this is not super reliable).

	2. Or, better yet do `fprintf(stderr, "..."`.

