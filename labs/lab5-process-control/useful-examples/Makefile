CFLAGS = -Wall -Og  -I../boot-check -Werror -std=gnu99
CC = gcc

PROGS = select-ex demand-ex pipe-ex signal-ex pipe-read-exit-ex wait-ex local-socket is-fd-open socket-ex
all : $(PROGS)

local-socket: local-socket.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

is-fd-open: is-fd-open.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

select-ex: select-ex.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

demand-ex: demand-ex.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

pipe-ex: pipe-ex.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

pipe-read-exit-ex: pipe-read-exit-ex.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

signal-ex: signal-ex.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

wait-ex: wait-ex.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

clean :
	rm -f *.o *~ Makefile.bak $(PROGS)

depend:
	makedepend *.[ch] 
