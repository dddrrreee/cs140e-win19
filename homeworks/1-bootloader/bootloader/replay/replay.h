// engler: consolidated definitions.
// you will implement:
//	- endpoint_t mk_endpoint_proc(const char *name, Q_t q, char *argv[]);
//	- void replay(endpoint_t *end, int corrupt_op);

#ifndef __REPLAY_H__
#define __REPLAY_H__

// the three operations we allow in the replay log.
enum { OP_READ32 = 11, OP_READ8, OP_WRITE32 };

// read in a replay log and return it.
struct Q read_input(FILE *in, int verbose_p);
const char *op_to_s(int op);

// the replay log.
// trivial Q operations.  should use the STL, but i don't want to assume C++.
typedef struct E {
        struct E *next;
        int op;
        unsigned cnt;
        unsigned val;
} E_t;

typedef struct Q {
        struct E *head, *tail;
	unsigned nreads, nwrites;
} Q_t;

struct Q mk_Q(void);
void Q_append(struct Q *q, int op, unsigned cnt, unsigned val);
struct E *Q_start(struct Q *q);
struct E *Q_next(struct E *e);
struct E *Q_pop(struct Q *q);

// simple endpoint.  name of process, its pid, the socket fd we use to 
// talk to/fro with.
typedef struct endpoint {
        const char *name;  	// name of process.
        int pid;		// its pid
        int fd;			// the bi-directional socket used to talk w/ it
        Q_t replay_log;		// the replay log we read in from input.
} endpoint_t;


// useful macros.
#define note(msg...) do {                       \
        fprintf(stderr, "REPLAY:");             \
        fprintf(stderr, ##msg);                 \
} while(0)

#define err(msg...) do {                       \
        fprintf(stderr, "REPLAY:ERROR:");       \
        fprintf(stderr, ##msg);                 \
        exit(1);                                \
} while(0)

endpoint_t mk_endpoint(const char *name, Q_t q, int fd, int pid);

// Your code
endpoint_t mk_endpoint_proc(const char *name, Q_t q, char *argv[]);
void replay(endpoint_t *end, int corrupt_op);

#endif

