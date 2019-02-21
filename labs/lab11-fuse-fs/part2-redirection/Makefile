# makefile based on scott mcpeak's grad school page :)
# 	http://scottmcpeak.com/autodepend/autodepend.html

NAME= redirect
WHERE=/tmp

OBJS = $(NAME).o  echo-until.o driver.o

CC = gcc
CFLAGS = -O -Wall -g -std=gnu99 -Wno-unused-function -Werror

# change these if this is not where you have them!
PI_SIDE_SHELL = ../../lab10-shell/shell-pi-side/pi-shell.bin
UNIX_SIDE_SHELL = ../../lab10-shell/shell-unix-side/pi-shell

all: test-redirect redirect test-pi-shell-redirect # run run.fake-pi

test-redirect: test-redirect.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

test-pi-shell-redirect:  test-pi-shell-redirect.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lm

run: 
	# if this test does not terminate, you did not close enough file descriptors
	./redirect ./test-redirect
 
run.fake-pi:
	./redirect ./test-pi-shell-redirect

run.pi:
	my-install -exec ./redirect $(UNIX_SIDE_SHELL) $(PI_SIDE_SHELL)

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
	rm -f $(NAME) *.o *.d Makefile.bak *~ *.bin tags test-redirect test-pi-shell-redirect
