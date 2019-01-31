

ifndef LIBPI_PATH
$(error LIBPI_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif
LPI= $(LIBPI_PATH)

CC = gcc
CFLAGS = -Wall -O -std=gnu99 -I$(LPI) -DFAKE_PI -I../pi-side-vmm


PI_SRC=my-gpio.c cs140e-timer.c cs140e-printf.c 

OBJS=$(PI_SRC:.c=.o)

all: fake-libpi.a

# 	objcopy fake-libpi.a --prefix-symbols="rpi_"

%.o: $(LPI)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<


fake-libpi.a:  $(OBJS) emulate-rpi.o main.o
	# rm -f fake-libpi.a
	ar cr fake-libpi.a $(OBJS) 
	objcopy fake-libpi.a --prefix-symbols="rpi_"
	objcopy fake-libpi.a --redefine-sym "rpi___stack_chk_fail=__stack_chk_fail"
	ar cr fake-libpi.a main.o emulate-rpi.o

clean :
	rm -f *.o *~ Makefile.bak fake-libpi.a

# run this to automatically derive include dependencies
depend:
	makedepend -I$(LPI) *.[ch] 
