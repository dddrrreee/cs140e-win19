# makefile based on scott mcpeak's grad school page :)
# 	http://scottmcpeak.com/autodepend/autodepend.html

NAME=pi-shell

BOOT_DIR = ../bootloader

OBJS = $(NAME).o shell-support.o \
	$(BOOT_DIR)/unix-side/simple-boot.o \
	$(BOOT_DIR)/unix-side/support.o\
	$(BOOT_DIR)/unix-side/trace.o

CC = gcc
CFLAGS = -Wall -g -std=gnu99  -I$(BOOT_DIR) -Wno-unused-variable -Wno-unused-function

all: make-deps $(NAME)

make-deps:
	make -C $(BOOT_DIR)/unix-side
	@ make -C $(BOOT_DIR)/pi-side
	make -C ../shell-pi-side

run:
	my-install -exec ./pi-shell ../shell-pi-side/pi-shell.bin

$(NAME):  $(OBJS) 
	$(CC) $(CFLAGS) $^  -o $@ -lm

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

ctags:
	ctags *.[ch]
# remove compilation products
clean:
	rm -f $(NAME) *.o *.d Makefile.bak *~ *.bin tags
