// engler: brain-dead queue.   should use STL but i don't want to force C++.

#include <assert.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>

#include "demand.h"
#include "replay.h"


struct Q mk_Q(void) {
        struct Q q;
        memset(&q, 0, sizeof q);
        return q;
}

struct E *Q_start(struct Q *q) {
        return q->head;
}
struct E *Q_next(struct E *e) {
        return e->next;
}

static int is_read(int op) {
	switch(op) {
	case OP_READ32: case OP_READ8: return 1;
	case OP_WRITE32: return 0;
	default: panic("bogus op: %d\n", op);
	}
}

struct E *Q_pop(struct Q *q) {
	// XXX should adjust nread/nwrite
        demand(q != 0, bad input);
        struct E *e = q->head;

        if(!e) {
                demand(!q->tail, invalid Q);
                return 0;
        }

        q->head = e->next;
        if(!q->head)
                q->tail = 0;
        return e;
}

void Q_append(struct Q *q, int op, unsigned cnt, unsigned val) {
        struct E *e = calloc(1, sizeof *e);
        e->cnt = cnt;
        e->op = op;
        e->val = val;

	if(is_read(op))
		q->nreads++;
	else
		q->nwrites++;

        if(q->tail) {
                q->tail->next = e;
                q->tail = e;
        } else {
                q->head = q->tail = e;
        }
}
