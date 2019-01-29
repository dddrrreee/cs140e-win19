#ifndef __TRACE_H__
#define __TRACE_H__


enum { TRACE_FD_REPLAY = 11, TRACE_FD_HANDOFF };

enum {
        TRACE_PUT32=1111,
        TRACE_GET32,
	TRACE_END,
};

// just turn on raw tracing.
void trace_turn_on_raw(void);

// -1 if not active, otherwise the current trace fd.
int trace_get_fd(void);

// these emit only if some form of tracing is active.

// call after reading nbytes.
void trace_read_bytes(unsigned char *buf, unsigned nybtes);

// call after writing a 32 bit
void trace_write32(unsigned u);

// call after reading a 32 bit
void trace_read32(unsigned u);


#endif
