// engler: dumb input parsing.  feel free to make better.
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "demand.h"
#include "replay.h"

static char *pop_field(char **line) {
	char *orig = *line;
	// scan up to ":" or space.

	for(char *p = orig; *p; p++) {
		if(*p == ':' || *p == ' ') {
			if(p == orig)
				panic("empty field: <%s>\n", orig);
			*p = 0;
			*line = p+1;
			return orig;
		}
	}
	return 0;
}

static unsigned pop_hex_num(char **line) {
	char *orig = *line;
	demand(*orig, impossible);

	char *num = pop_field(line);
	if(!num)
		panic("corrupt file: expected a field, have <%s>\n", orig);

	for(char *p = num; *p; p++) {
		if(!isxdigit(*p))
			panic("input corrupt: expected number, have <%s>\n", 
					orig);
	}
	return strtol(num, 0, 16);
}

static unsigned pop_num(char **line) {
	char *orig = *line;
	demand(*orig, impossible);

	char *num = pop_field(line);
	if(!num)
		panic("corrupt file: expected a field, have <%s>\n", orig);

	for(char *p = num; *p; p++) {
		if(!isdigit(*p))
			panic("input corrupt: expected number, have <%s>\n", 
					orig);
	}
	return strtol(num, 0, 10);
}

static char *expect(char *input, const char *exp) {
	char *p = input;
	char *field = pop_field(&p);
	if(!field)
		panic("expected <%s>, have nothing: <%s>\n", exp,input);
	if(strcmp(field, exp) != 0)
		panic("expected <%s>, have <%s>\n", exp, field);
	return p;
}


const char *op_to_s(int op) {
	switch(op) {
	case OP_WRITE32: return "WRITE32";
	case OP_READ32: return "READ32";
	case OP_READ8: return "READ8";
	default: panic("invalid op=%d\n", op);
	}
}

struct Q read_input(FILE *in, int emit_p) {
	char buf[1024];

	struct Q q = mk_Q();
	
	for(int nlines = 0; fgets(buf, sizeof buf, in); nlines++) {
		char input[1024];
		strcpy(input,buf);
		char *p = expect(buf, "TRACE");
		char *op_s = pop_field(&p);

		int op = 0;
		if(strcmp(op_s, "WRITE32") == 0)
			op = OP_WRITE32;
		else if(strcmp(op_s, "READ32") == 0)
			op = OP_READ32;
		else if(strcmp(op_s, "READ8") == 0)
			op = OP_READ8;
		else
			panic("input corrupt: expected WRITE32 or READ32, got=<%s>\n", op_s);

		unsigned cnt = pop_num(&p);
		if(cnt != nlines)
			panic("input cnt=%d, nlines=%d, buf=<%s>\n", cnt, nlines, input);

		unsigned u = pop_hex_num(&p);

		if(emit_p) 
			printf("TRACE:%s:%d:%x %s", 
				op_to_s(op),
				nlines, 
				u,
				p);
		Q_append(&q, op, nlines, u);
	}
	return q;
}

#ifdef TEST
/*
 * Simple test:
 *	gcc -Wall -O -DTEST scan.c
 *	./a.out < hello.trace.txt > t.txt
 *  	diff hello.trace.txt t.txt
 */
int main(void) { 
	struct Q q = read_input(stdin, 1);

	for(struct E *e = Q_start(&q); e; e = Q_next(e)) 
		fprintf(stderr, "%s:%d:%x\n", op_to_s(e->op), e->cnt, e->val);
	struct E *e;

	while((e = Q_pop(&q)))
		fprintf(stderr, "%s:%d:%x\n", op_to_s(e->op), e->cnt, e->val);
	return 0;
}
#endif
