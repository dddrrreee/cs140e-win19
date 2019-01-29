CC = gcc
CFLAGS = -Wall -g -std=gnu99 -I../unix-side
OBJS = ../unix-side/support.o ../unix-side/tty.o

RUN = ../unix-side/my-install ../test-binaries/hello.bin
RUN_TRACE = ../unix-side/my-install -trace ../test-binaries/hello.bin

PROGS = replay hand-off

all : support $(PROGS) run # test.hand-off # run

support:
	@make -C ../unix-side my-install

replay:  replay.o Q.o parse-input.o  replay-driver.o
	$(CC) $(CFLAGS) $^  -o $@ -lm

hand-off:  $(OBJS)  hand-off.o
	$(CC) $(CFLAGS) $^  -o $@ -lm

# can modify replay to emit its own traces.
run: 
	./replay $(RUN) < ../unix-side/hello.trace.txt 

test.hand-off:
	./hand-off $(RUN_TRACE) | grep TRACE: > /tmp/trace.txt
	diff /tmp/trace.txt ../unix-side/hello.trace.txt

clean :
	rm -f *.o *~ Makefile.bak $(PROGS)

# run this to automatically derive include dependencies
depend:
	makedepend -I../unix-side *.[ch] 
