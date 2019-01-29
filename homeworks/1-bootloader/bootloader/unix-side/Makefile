CFLAGS = -Wall -Werror -g 
CC = gcc
SRC = my-install.c simple-boot.c  support.c trace.c tty.c
OBJS = $(SRC:.c=.o)

all : my-install  test

my-install:  $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o my-install -lm

test:
	./my-install -trace ../test-binaries/hello.bin |grep TRACE: > /tmp/trace.txt
	diff /tmp/trace.txt hello.trace.txt
	@cksum /tmp/trace.txt | awk '{print $$1;}' > /tmp/hash.txt
	diff hello.hash.txt  /tmp/hash.txt


mktest:
	./my-install -trace ../test-binaries/hello.bin |grep TRACE: > hello.trace.txt
	cksum hello.trace.txt | awk '{print $$1;}' > hello.hash.txt

clean :
	rm -f *.o *~ Makefile.bak my-install

depend:
	makedepend *.[ch] 
