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


## 1. Externalize writes.

In lab 2 we saw how to trace read and writes and use these traces to
show code equivalance.  We're going to do a variation of the same
hack to show that everyone's bootloader is equivalant.

This first step is easy:
	1. In your `simple-boot.c` implementation, insert call to 
	`trace_read32(u)` in your `get_uint` code and a call to
	`trace_write32(u)` in your `put_uint` code.

	2. Run `my-install -trace ../test-binaries/hello.bin` 
	to see all the reads and writes done.

	3. Grep out the trace commands and put them in a file so you
	can compare to everyone else.  And so you can compare to 
	subsequent modifications.


At this point, you probably have different results than other people.
Change your code if needed, or get them to change theirs.


## 2. Do a hand-off.

The full part of this lab will be to use an external process to 
replay the log you saved above (every line with `TRACE:` in it),
testing that your `my-install` always does the same thing, and
then re-running it, corrupting each individual read, checking that
it detects the corruption and exits.

The first basic step for this is to be able to create a subprocess
and communicate with it over a socket.  For this part:

	1. Look in `replay/hand-off.c`
	2. Implement `run` and `exit_code`.  The programsin `useful-examples`
	in the lab directory will likely help.
	3. Make sure the test `make test.hand-off` is successful.


	3. This will print out all the reads and writes done by the 
	Unix side.

	2. Run `my-install` with the trace option.

	3. This will print out all the reads and writes done by the 
	Unix side.


## 3. Do a sucessful run.

In this step, you will implement `replay` in `replay.c` so that it 
will take a log and re-run it over the socket, making sure the process
exits successfully.  The code has comments on what to do.

## 4. Do unsuccessful runs

In this step, you will extend `replay` to corrupt the value in the 
`corrupt_op` log entry and check that the code exits with a 
non-zero exit code.

At this point, your code should be in good shape.  
