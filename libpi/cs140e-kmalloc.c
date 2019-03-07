#include "rpi.h"


#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))
#define is_aligned(x, a)        (((x) & ((typeof(x))(a) - 1)) == 0)
union align {
        double d;
        void *p;
        void (*fp)(void);
};


extern char __heap_start__;
static char *heap = &__heap_start__;

void *kmalloc_heap_end(void) { return heap; }
void *kmalloc_heap_start(void) { return &__heap_start__; }

void kmalloc_set_start(unsigned _addr) {
    char *addr = (void*)_addr;
    assert(addr > heap);
    heap = addr;
}

void *kmalloc(unsigned sz) {
        sz = roundup(sz, sizeof(union align));

        void *addr = heap;
        heap += sz;

        memset(addr, 0, sz);
        return addr;
}

#define is_pow2(x)  (((x)&-(x)) == (x))

void *kmalloc_aligned(unsigned nbytes, unsigned alignment) {
    demand(is_pow2(alignment), assuming power of two);
    
    // XXX: we waste the alignment memory.  aiya.
    //  really should migrate to the k&r allocator.
    unsigned h = (unsigned)heap;
    h = roundup(h, alignment);
    demand(is_aligned(h, alignment), impossible);
    heap = (void*)h;

    return kmalloc(nbytes);
}

void kfree(void *p) { }
void kfree_all(void) { heap = &__heap_start__; }
void kfree_after(void *p) { heap = p; }

