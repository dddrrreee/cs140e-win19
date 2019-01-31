ifndef NAME
$(error NAME is not set: give the name of the binary to make)
endif

CC = gcc

ifndef LIBPI_PATH
$(error LIBPI_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

PI_VMM = ../unix-side-vmm/


CC = gcc
CFLAGS = -Wall -g -Og -std=gnu99 -I$(LPI) -DFAKE_PI

LPI= $(LIBPI_PATH)

all: $(NAME).fake

%.fake.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
	objcopy $@ --prefix-symbols="rpi_"

$(NAME).fake:  $(NAME).fake.o $(PI_VMM)/fake-libpi.a
	$(CC) $(CFLAGS) $(NAME).fake.o -o $@ $(PI_VMM)/fake-libpi.a

clean :
	rm -f *.o *~ Makefile.bak $(NAME).fake
