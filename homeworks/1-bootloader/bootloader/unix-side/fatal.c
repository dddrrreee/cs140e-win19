#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "demand.h"

void FFatal(const char *file, int line, const char *fmt, ...) {
        va_list args;
        fprintf(stderr, "FATAL:%s:%d:", file, line);
        va_start(args, fmt);
        	vfprintf(stderr, fmt, args);
        va_end(args);
	// fprintf(stderr, "\terrno %d = <%s>\n", errno, strerror(errno));
        exit(1);
}
