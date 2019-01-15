CFLAGS = -Wall -Og 
CC = gcc
SRC = my-install.c fatal.c simple-boot.c  support.c
OBJS = $(SRC:.c=.o)

all : my-install 

my-install:  $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o my-install -lm

clean :
	rm -f *.o *~ Makefile.bak my-install

depend:
	makedepend *.[ch] 
