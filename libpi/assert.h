#ifndef PRINTF_DEFS
#define PRINTF_DEFS

#ifndef NDEBUG
#define debug(msg, args...) \
	(printf)("%s:%s:%d:" msg, __FILE__, __FUNCTION__, __LINE__, ##args)
#else
#define debug(msg, args...) do { } while(0)
#endif

#define panic(msg, args...) do { 					\
	(printf)("PANIC:%s:%s:%d:" msg "\n", __FILE__, __FUNCTION__, __LINE__, ##args); \
	reboot();							\
} while(0)

#define assert(bool) do { if((bool) == 0) panic(#bool); } while(0)
#endif
