# makefile based on scott mcpeak's grad school page :)
# 	http://scottmcpeak.com/autodepend/autodepend.html

NAME= hello
WHERE=./fuse-hello

OBJS = $(NAME).o 

FUSE_CMD = ./$(NAME) -f -d -s $(WHERE)

CC = gcc
CFLAGS = -O -Wall -g -std=gnu99 -D_FILE_OFFSET_BITS=64   -Wno-unused-function     `pkg-config fuse --cflags --libs`

all: check_os $(NAME) 

check_os:
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
        LINUX_LIBS=-lfuse
endif

run:
	cat $(WHERE)/hello
	sleep 1
	cat $(WHERE)/hello

mount: 
	mkdir -p $(WHERE)
	./$(NAME) -f -d -s $(WHERE)

unmount:
	fusermount -uz $(WHERE)

$(NAME):  $(OBJS) 
	$(CC) $(CFLAGS) $^ `pkg-config fuse --cflags --libs`  -o $@ -lm  $(LINUX_LIBS)


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
