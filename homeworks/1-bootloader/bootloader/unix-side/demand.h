#ifndef __DEMAND_H__
#define __DEMAND_H__

#include <stdio.h>
#include <stdlib.h>

#define _XSTRING(x) #x

// usage: 
//	demand(expr, msg)
// prints <msg> if <expr> is false.
// example:
//	demand(x < 3, x has bogus value!);
// note: 
//	if <_msg> contains a ',' you'll have to put it in quotes.
#define demand(_expr, _msg) do {					\
	if(!(_expr)) { 							\
		fprintf(stderr, "ERROR:%s:%s:%d: "			\
			"FALSE(<" _XSTRING(_expr) ">): " _XSTRING(_msg) "\n",\
			__FILE__, __FUNCTION__, __LINE__);		\
		exit(1);						\
	}								\
} while(0)


#define sys_die(syscall, msg) do {                                      \
        fprintf(stderr, "%s:%s:%d: <%s>\n\tperror=",                    \
                        __FILE__, __FUNCTION__, __LINE__, _XSTRING(msg));               \
        perror(_XSTRING(syscall));                                      \
        exit(1);                                                        \
} while(0)

#define panic(msg...) do { 						\
        fprintf(stderr, "%s:%s:%d:", __FILE__, __FUNCTION__, __LINE__); \
        fprintf(stderr, ##msg);						\
        exit(1);                                                        \
} while(0)

#define debug(msg...) do { 						\
        fprintf(stderr, "%s:%s:%d:", __FILE__, __FUNCTION__, __LINE__); \
        fprintf(stderr, ##msg);						\
} while(0)

/* Compile-time assertion used in function. */
#define AssertNow(x) switch(1) { case (x): case 0: ; }

#define unimplemented() panic("implement this function!\n");
#endif /* __DEMAND_H__ */
