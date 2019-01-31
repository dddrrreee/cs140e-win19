#############################################################################
# We provide any file prefixed with cs140e- (e.g., cs140e-printf.c).  You
# can replace these if you want.
#
# After each lab, just add any .c files you created here, so that all your
# code can use it.
#
# TODO: You should replace these .o's with your code and remove
# the use of SUPPORT_OBJS
SUPPORT_OBJS= cs140e-gpioextra.o cs140e-uart.o 

START= cs140e-start.o

# add your new files here.
# e.g., my-uart.c
SRC = 	cs140e-printf.c  	\
	cs140e-reboot.c  	\
	cs140e-timer.c 		\
	cs140e-mem-barrier.s  	\
	cs140e-stdlib.c		\
	cs140e-cstart.c		\
	my-gpio.c		\
	cs140e-put-get.s

# defines CC, etc.
include includes.mk

ifndef LIBPI_PATH
$(error LIBPI_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

LIB_OBJS := $(SRC:.c=.o)
LIB_OBJS := $(LIB_OBJS:.s=.o)

TARGET = libpi.a

all : $(TARGET) $(START)

.s.o:
	$(AS) $(ASFLAGS) $< -o $@ 
.c.o:
	$(CC) $(CFLAGS) -c $<  -o $@

# we bundle start.s with the library: just make different ones for each
# different start so that the library is internally consistent.
$(TARGET): $(LIB_OBJS) $(SUPPORT_OBJS)
	arm-none-eabi-ar crf $(TARGET) $^ 
	$(OD) -D $@ > $@.list

clean :
	rm -f $(LIB_OBJS) cs140e-start.o *.bin *.hex *.elf *.list *.img *~ $(TARGET)  Makefile.bak 

depend: 
	makedepend $(SRC)
