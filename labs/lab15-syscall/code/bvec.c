#include "rpi.h"
#include "bvec.h"

#if 0
typedef struct {
    uint32_t n_tot, n_left;
    uint32_t *elm;
} bvec_t;
#endif

uint32_t bvec_alloc(bvec_t *b) {
    if(b->n_left == 0)
        return -1;
    demand(b->n_left > 0, ran out!);
    return b->elm[--b->n_left];
}
void bvec_free(bvec_t *b, uint32_t x) {
    demand(b->n_left < b->n_tot, too many freed);

    for(int i = 0; i < b->n_left; i++)
        if(b->elm[i] == x)
            panic("double free of %x\n",x);
    b->elm[b->n_left++] = x;
}
// allocate integers [lb,ub) 
bvec_t bvec_mk(uint32_t lb, uint32_t ub) {
    assert(ub>lb);
    unsigned n = ub - lb;

    bvec_t b = (bvec_t) { .n_left = 0, .n_tot = n };
    b.elm = kmalloc(n * sizeof *b.elm);
    for(int i = 0; i < n; i++)
        bvec_free(&b, ub-i-1);
    return b;
}
void bvec_print(const char *msg, bvec_t *b) {
    printk("%s\n", msg);
    for(int i = 0; i < b->n_left; i++)
        printk("b[%d]=%d\n",i,b->elm[i]);
    printk("\n");
}
