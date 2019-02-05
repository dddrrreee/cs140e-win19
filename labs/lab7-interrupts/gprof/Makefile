# makefile based on scott mcpeak's grad school page :)
# 	http://scottmcpeak.com/autodepend/autodepend.html

NAME=gprof
TI= ../timer-int/
OBJS = $(NAME).o 		\
	$(TI)/interrupts-asm.o	\
	 $(TI)/interrupts-c.o	\
	 $(TI)/cstart.o		\
	 $(TI)/start.o		\
	 $(TI)/timer-interrupt.o

ifndef LIBPI_PATH
$(error LIBPI_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

# useful set of defines.
include $(LIBPI_PATH)/includes.mk

CFLAGS += -I$(TI)

all: libpi $(NAME).bin

# is there a better way to do this?
libpi:
	@make -C $(LIBPI_PATH)

$(NAME).bin: $(TI)/memmap $(OBJS)
	$(LD) $(OBJS) -T $(TI)/memmap -o $(NAME).elf  $(LPI)
	$(OD) -D $(NAME).elf > $(NAME).list
	$(OCP) $(NAME).elf -O binary $(NAME).bin

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

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

# remove compilation products
clean:
	rm -f replay *.o *.d *.bin *.elf *.list *.img Makefile.bak *~
