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

// use this for timeouts.
static unsigned timeout_secs = 2;

// given unsigned <v> corrupt it so that <v_new> != <v>
static unsigned corrupt32(unsigned v) {
	unsigned c;
	while((c = random()) == v)
		;
	return c;
}

/*
 * run the unix side of bootloader (my-install) as a subprocess where
 * you can talk to it over a socket.
 *
 *	1. create a socket pair.
 *	2. fork, store child pid in <pid>
 *	3. in child:
 *		a. make sock[1] into file descriptor <TRACE_FD_REPLAY>
 *		b. close the unused socket side.
 *		c. execvp using argv[]
 *	4. in parent:
 *		a. close child side socket.
 *		b. make an endpoint.
 *		c. return endpoint.
 */
endpoint_t mk_endpoint_proc(const char *name, Q_t q, char *argv[]) {
	int pid;
	int sock[2];

	unimplemented();

	// this should sort-of follow your handoff code except you're using
	// sockets.

        return mk_endpoint(name, q, sock[0], pid);
}

/*
 * synchronously wait for the child <this> to complete.  
 * 	1. if exited cleanly, return it's error code.
 *	2. otherwise return -1.
 */
static int proc_exit_code(endpoint_t *this) {
	// use your hand-off code (more or less)
	unimplemented();
}

static void write_exact(endpoint_t *this, void *buf, int nbytes) {
        int n;
        if((n = write(this->fd, buf, nbytes)) < 0)
                panic("i/o error writing to <%s> = <%s>\n",
                        this->name, strerror(errno));
        demand(n == nbytes, something is wrong);
}

/*
 * Use select to check if the this->fd has data.
 *  	1. return 0 if there is a timeout.
 *	2. return 1 if not.
 */
int has_data(endpoint_t *this, unsigned timeout_secs) {
	unimplemented();
        return 1;
}

/*
 * Check if <end> has an eof.
 *
 * Note, for later: when a process dies, the 0 bytes EOF is "written" 
 * to the socket/pipe. A read() of EOF = 0 bytes.
 *
 * 	1. see if there is data (EOF is data)
 * 	2. read() it if so and verify is EOF.
 *	3. give an error if not EOF.
 *	4. otherwise return 1.
 *
 * Make sure you handle the case that the read() blocks!!
 */
static int is_eof(endpoint_t *end) {
	if(!has_data(end, timeout_secs))
		err("process <%s> should have exited after 1 sec\n", end->name);
	unimplemented();
	return 1;
}

/*
 * Read until you get <n> bytes or 0.
 *	- give an err() if read fails (read() < 0).  EXCEPT: if <can_fail_p>
 *	is != 0.  in which case just return 0.
 *
 * NOTE: the unix side may be sending 1 byte at a time.  Thus, if we read
 * > 1 bytes, we might get a short read.  Thus, you have to keep iterating
 * until you get <n> bytes or there's a fatal error.
 */
// endpoints write 1 byte at a time.  so can get a split.
static int read_exact(endpoint_t *e, void *buf, int n, int can_fail_p) {
	unimplemented();
	return 1;
}

// run the replay log.  if the replay'd code is determinist, will behave
// the same.  
//
// an interesting thing is that the overall system (your laptop) is very
// non-deterministic and, even if it was determ, its state changes all the
// time.  however, b/c of interfaces (and their contracts) we can have
// deterministic code in the midst of this chaos.
void replay(endpoint_t *end, int corrupt_op) {
	int can_fail_p = 0;
	int status = 0;

	E_t *e = Q_start(&end->replay_log);
	for(int n = 0; e; e = Q_next(e), n++) {
		// note("about to do op= <%s:%d:%x>\n", op_to_s(e->op), e->cnt, e->val);

		unsigned v;

		// polarity of read/write will depend on if the unix
		// or pi side is sending.   right now we just test 
		// unix sending.  so when log says WRITE32, we have
		// to read and when READ32, we have to write.
		switch(e->op) {
		case OP_READ8:
		{
			unimplemented();
			break;
		}

		// replay'd process is GET32'ing, so we write to socket.
		case OP_READ32:
		{
			unimplemented();
			break;
		}
		// replay'd process is PUT32'ing, so we read from socket.
		case OP_WRITE32:
			assert(n != corrupt_op);
			unimplemented();
			break;
		default: panic("invalid op <%d>\n", e->op);
		}

		// note("success: matched %s:%d:%x\n", op_to_s(e->op), e->cnt, e->val);
	}

	// successfully consumed the log.
	if(can_fail_p) {
		// I believe this can only happen on the very last corruption.
		note("process completed its input and did not fail?\n");
		goto error;
	}

	if(!is_eof(end))
		panic("expected eof: got nothing\n");
	else
		note("successful EOF\n");

	// We didn't corrupt anything: should exit with 0. 
	// NOTE: if the process is in an infinite loop we will get stuck.
	if((status = proc_exit_code(end)) != 0)
		err("process exited with %d, expected 0\n", status);
	else
		note("SUCCESS: process exited with %d\n", status);
	return;

error:
	// hit an error case.
	demand(can_fail_p, something wrong);
	if((status = proc_exit_code(end)) == 0)
		err("process exited successfully, expected an error\n");
	else
		note("SUCCESS: after corrupt, process exited with %d\n", status);

}
