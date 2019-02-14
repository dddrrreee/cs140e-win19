#include "rpi.h"

#define aligned(ptr, n)  ((unsigned)ptr % n == 0)
#define aligned4(ptr)  aligned(ptr,4)

void *memset(void *_p, int c, size_t n) {
        char *p = _p, *e = p + n;

        while(p < e)
                *p++ = c;
        return _p;
}

int memcmp(const void *_s1, const void *_s2, size_t nbytes) { 
	const unsigned char *s1 = _s1, *s2 = _s2;

	for(int i = 0; i < nbytes; i++) {
		int v = s1[i] - s2[i];
		if(v)
			return v;
	}
	return 0;
}

void *memcpy(void *dst, const void *src, size_t nbytes) { 
	// this is not for optimization.   when gcc copies structs it may
	// call memcpy.   if the dst struct is a pointer to hw, and we
	// do byte stores, i don't think this will necessarily lead to 
	// good behavior.
	if(aligned4(dst) && aligned4(src) && aligned4(nbytes)) {
        	unsigned n = nbytes / 4;
        	unsigned *d = dst;
        	const unsigned *s = src;

        	for(unsigned i = 0; i < n; i++)
			d[i] = s[i];
	} else {
        	unsigned char *d = dst;
        	const unsigned char *s = src;
		for(unsigned i = 0; i < nbytes; i++)
			d[i] = s[i];
	}
	return dst;
}

int strcmp(const char *a, const char *b) {
        while (*a && *a == *b)
                ++a, ++b;
        return *a - *b;
}

// https://clc-wiki.net/wiki/strncmp#Implementation
int strncmp(const char* _s1, const char* _s2, size_t n) {
	const unsigned char *s1 = (void*)_s1, *s2 = (void*)_s2;
	while(n--) {
        	if(*s1++!=*s2++)
            		return s1[-1] - s2[-1];
	}
    	return 0;
}

// use this if you need memory barriers.
void dev_barrier(void) {
        dmb();
        dsb();
}

