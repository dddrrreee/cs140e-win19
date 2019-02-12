#ifndef __Q_H__
#define __Q_H__

#ifndef E
#	error "Client must define the Q datatype <E>"
#endif

// engler: brain-dead generic queue. don't want to use STL/c++ in the kernel.

typedef struct Q {
        E *head, *tail;
        unsigned n;
} Q_t;

static unsigned Q_size(struct Q *q) {
	return q->n;
}

static E *Q_start(struct Q *q) { 
        return q->head;
}
static E *Q_next(E *e) {
        return e->next;
}

static E *Q_pop(struct Q *q) {
        demand(q != 0, bad input);
        E *e = q->head;

        if(!e) {
                demand(!q->tail, invalid Q);
                return 0;
        }
        q->head = e->next;
        if(!q->head)
                q->tail = 0;
        return e;
}

static void Q_append(struct Q *q, E *e) {
	e->next = 0;
        if(!q->tail) 
                q->head = q->tail = e;
        else {
                q->tail->next = e;
                q->tail = e;
        }
}

#endif
