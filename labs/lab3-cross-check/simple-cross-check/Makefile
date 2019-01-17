CFLAGS = -Wall -Og   -std=gnu99  
CC = gcc
SRC = simple-check.c fake-pi.c blink.c
OBJS = $(SRC:.c=.o)


all : test-blink

test-blink:  $(OBJS)   test-blink.o
	$(CC) $(CFLAGS) $(OBJS) test-blink.o -o test-blink -lm

clean :
	rm -f *.o *~ Makefile.bak test-blink

depend:
	makedepend *.[ch] 
