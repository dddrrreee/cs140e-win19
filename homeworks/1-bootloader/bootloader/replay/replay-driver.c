// engler: driver for your replay() code.
// 
// 	- void check_success(Q_t replay_log, char *argv[]) runs <argv>
//	with no errors.
//
// 	- void check_crashes(Q_t replay_log, char *argv[]) runs <argv>
//	failing a single operation at a time.  (each of these should 
//	cause the process <argv> to abort or exit w/ a non-zero exit code.

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <fcntl.h>

#include "demand.h"
#include "replay.h"
#include "trace.h"

// Ugh.  You need all the reads and writes that go back and forth.
// Need to change echo if you want this to happen.  It's feasible ---
// just do GET8.

endpoint_t mk_endpoint(const char *name, Q_t q, int fd, int pid) {
	return (endpoint_t) 
	{ 
		.name = strdup(name), 
		.replay_log = q,
		.fd = fd,
		.pid = pid
	};
}

static void destroy_end(endpoint_t *e) {
	close(e->fd);
	kill(e->pid, SIGKILL);
}



/* 
 * systematically corrupt one operation at a time and see that the unix
 * side fails.  [extend this to the pi side!].
 *
 * NOTE:
 *	- we only want to corrupt bytes in the protocol --- the other bytes
 *	are unchecked by the bootloader code, so corrupting them 
 *	will not lead to errors we care about.
 *
 * 	- the most general way to do this is to have the checked program
 *	emit CORRUPT_START / CORRUPT_END flags so it can turn on corruption
 *	for the bytes it cares about.
 *
 *	- to minimize touching the bootloader (and for expediency) we do
 *	do a gross hack: the protocol uses WRITE32/READ32, the non-checked 
 *	reads are READ8 (which it just echos to the screen).  So
 *	we just march down corrupting READ32s.
 */
void check_crashes(Q_t replay_log, char *argv[]) {

	// if the child dies while we are writing a socket, we will actually
	// (sometimes) get a SIGPIPE.
	if(signal(SIGPIPE, SIG_IGN) < 0)
                sys_die(signal, cannot catch);

	E_t *e = Q_start(&replay_log);
	for(int n = 0; e; e = Q_next(e), n++)  {
		// once we hit a READ8 we know we are done with the 
		// protocol.
		if(e->op == OP_READ8)
			break;
		// we don't corrupt writes since there is no-one to act
		// on these.
		if(e->op == OP_WRITE32)
			continue;
		
		demand(e->op == OP_READ32, invalid op);
		note("ATTEMPT: going to corrupt op %d\n", n);

		// create a new process.
		endpoint_t end = 
			mk_endpoint_proc("unix.side", replay_log, &argv[1]);

		// check it.
		replay(&end, n);

		// clean up.
		destroy_end(&end);
	}
}

// check no failure.
void check_success(Q_t replay_log, char *argv[]) {
	endpoint_t end = mk_endpoint_proc("unix.side", replay_log, &argv[1]);
	replay(&end, -1);
	destroy_end(&end);
}

/*
 * Simple test:
 *	gcc -Wall -O -DTEST scan.c
 *	./a.out < hello.trace.txt > t.txt
 *  	diff hello.trace.txt t.txt
 */
int main(int argc, char *argv[]) { 
	demand(argc > 1, no arguments?);

	const int N = 1;
	srandom(0); 	// so everyone has the same.
	struct Q q = read_input(stdin, 0);

	// run the successful attempt N times --- should be the same.
	for(int i = 0; i < N; i++)
		check_success(q, argv);

	// now try failures.  we iterate these N times as well, should
	// get the same fails.
	//
	// NOTE: you can externalize the reads and writes and then 
	// check against everyone else.
	for(int i = 0; i < N; i++)
		check_crashes(q, argv);

#if 0
	for(struct E *e = Q_start(&end.replay_log); e; e = Q_next(e)) 
		fprintf(stderr, "%s:%d:%x\n", op_to_s(e->op), e->cnt, e->val);
	struct E *e;

	while((e = Q_pop(&q)))
		fprintf(stderr, "%s:%d:%x\n", op_to_s(e->op), e->cnt, e->val);
#endif
	return 0;
}
