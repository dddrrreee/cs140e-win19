#include "rpi.h"


#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))
union align {
        double d;
        void *p;
        void (*fp)(void);
};


extern char __heap_start__;
static char *heap = &__heap_start__;

void *kmalloc_heap_end(void) { return heap; }
void *kmalloc_heap_start(void) { return &__heap_start__; }

void *kmalloc(unsigned sz) {
        sz = roundup(sz, sizeof(union align));

        void *addr = heap;
        heap += sz;

        memset(addr, 0, sz);
        return addr;
}
void kfree(void *p) { }
void kfree_all(void) { heap = &__heap_start__; }
void kfree_after(void *p) { heap = p; }

