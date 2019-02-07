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
	cs140e-rand.c		\
	my-gpio.c		\
	cs140e-pwm.c		\
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


# we bundle start.s with the library: just make different ones for each
# different start so that the library is internally consistent.
$(TARGET): $(LIB_OBJS) $(SUPPORT_OBJS)
	arm-none-eabi-ar crf $(TARGET) $^ 
	$(OD) -D $@ > $@.list


# pull in dependency info for *existing* .o files
-include $(LIB_OBJS:.o=.d)

# compile and generate dependency info;
# more complicated dependency computation, so all prereqs listed
# will also become command-less, prereq-less targets
#   sed:    strip the target (everything before colon)
#   sed:    remove any continuation backslashes
#   fmt -1: list words one per line
#   sed:    strip leading spaces
#   sed:    add trailing colons
%.o: %.c
	$(CC) -c $(CFLAGS) $*.c -o $*.o
	$(CC) -MM $(CFLAGS) $*.c > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

.s.o: 
	$(AS) $(ASFLAGS) $< -o $@


clean :
	rm -f $(LIB_OBJS) cs140e-start.o *.bin *.hex *.elf *.list *.img *~ $(TARGET)  Makefile.bak  *.d

depend: 
	makedepend $(SRC)
