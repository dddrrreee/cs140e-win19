## Lab: testing your bootloader using process control and fault-injection.

This lab will show how to use Unix verbs (`fork`, `exec`, `waitpid`)
and nouns (sockets, pipes, processes) to control processes and make them
do tricks.

Our main trick will be testing your bootloader code by using another
process to intercept all its communication and cause it to fail in a
variety of ways, making sure it does not crash.

Since man pages are not always clear, the sub-directory 
`useful-examples` has a bunch
of concrete examples showing how to use the system calls you'll need.
You should go through and read/run/modify the various programs to get
a reasonable feel for how things work.

### Don't make these mistakes.

The single largest mistake I see people do over and over again in lab is not
checking if a system call failed.  Given that this is many people's
first experience with these calls, the probability that they are used
incorrectly is 1.  Further, given that we are using hardware, the probability
that a call fails because of a pi issue is 2.  Checking calls
is a selfish rather
an a priggish moral issue: it is the
simplest, fastest way to detect when you've screwed up.  It will save
you a huge amount of time, since otherwise you have to invert the
function "my program doesn't work" which is wildly, tediously non-biijective.
Such checking is not hard.  You can do something as simple as:

    #include "demand."

    ...
    if(write(fd, buf, n) < 0)
          sys_die(write, write failed?)

Which will kill your program, printing out: (1) the file, function and
line number of the call, (2) the actual error the system call returned
and (3) printing out any helpful message you want.

A second, much more minor gotcha: if you are using `printf` to see what's
going on, both MacOS and Linux will often buffer the output --- i.e., not
print it til "later."
This can be confusing since you will then believe code has not run when
in fact it has.  To get around this either:

	1.  Put a newline on any `printf` (not guaranteed).

	2. Or, better yet do `fprintf(stderr, "..."` which will immediately
	force output to `stderr`.  There's a helpful macro `debug` in 
	`demand.h` which will do this.

