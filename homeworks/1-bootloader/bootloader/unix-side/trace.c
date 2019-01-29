// example of using function pointers to get rid of if-statements 
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <assert.h>
#include "../shared-code/simple-boot.h"
#include "demand.h"
#include "trace.h"

static int trace_fd = -1;
static int trace_init_p = 0;

/************************************************************************
 * Trace v0: immediately print out the operation/value.  We make a weak 
 * attempt to annotate the bytes being sent to help debugging.
 */
static const char *cmd_to_str(unsigned u) {
        switch(u) {
        case ARMBASE:   return "ARMBASE?";
        case SOH:       return "SOH?";
        case BAD_CKSUM: return "BAD_CKSUM?";
        case BAD_START: return "BAD_START?";
        case BAD_END:   return "BAD_END?";
        case TOO_BIG:   return "TOO_BIG?";
        case ACK:       return "ACK?";
        case NAK:       return "NAK?";
        case EOT:       return "EOT?";
        default:        return "DATA?";
        }
};

static int n_offset = 0;
void trace_emit_read32(unsigned u) {
        printf("TRACE:READ32:%d:%x [%s]\n", n_offset++, u, cmd_to_str(u));
}
void trace_emit_write32(unsigned u) {
	printf("TRACE:WRITE32:%d:%x [%s]\n", n_offset++, u, cmd_to_str(u));
}
void trace_emit_read8(unsigned char c) {
       	printf("TRACE:READ8:%d:%x [bytes]\n", n_offset++, c);
}

// if either we are doing raw tracing or a simple bypass.

void trace_read32(unsigned u) {
	if(trace_init_p)
		trace_emit_read32(u);
}

void trace_write32(unsigned u) {
	if(trace_init_p)
        	trace_emit_write32(u);
}

void trace_read_bytes(unsigned char *p, unsigned nbytes) {
	if(trace_init_p)
		for(unsigned i = 0; i < nbytes; i++)
			trace_emit_read8(p[i]);
}

/***********************************************************************
 * General trace code.
 */

int trace_get_fd(void) {
	static int init_p = 0;
	if(!init_p) {
		init_p = 1;
    		if(fcntl(TRACE_FD_REPLAY, F_GETFL) >= 0)
			trace_fd = TRACE_FD_REPLAY;
    		else if(fcntl(TRACE_FD_HANDOFF, F_GETFL) >= 0)
			trace_fd = TRACE_FD_HANDOFF;
	}
	return trace_fd;
}

// turn on raw tracing.
void trace_turn_on_raw(void) {
	assert(!trace_init_p);
	trace_init_p = 1;
}
