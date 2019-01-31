CC = gcc
CFLAGS = -Wall -g -std=gnu99 -I../unix-side
OBJS = ../unix-side/support.o ../unix-side/tty.o

RUN = ../unix-side/my-install ../test-binaries/hello.bin
RUN_TRACE = ../unix-side/my-install -trace ../test-binaries/hello.bin

PROGS = echo

all : support $(PROGS) run # test.hand-off # run

support:
	@make -C ../unix-side my-install

echo:  $(OBJS) echo.o
	$(CC) $(CFLAGS) $^  -o $@ -lm

# can modify replay to emit its own traces.
run: 
	../unix-side/my-install -exec ./echo ../test-binaries/hello.bin

clean :
	rm -f *.o *~ Makefile.bak $(PROGS)

# run this to automatically derive include dependencies
depend:
	makedepend -I../unix-side *.[ch] 
