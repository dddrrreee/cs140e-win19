#ifndef __BVEC_H__
#define __BVEC_H__
typedef struct {
    uint32_t n_tot, n_left;
    uint32_t *elm;
} bvec_t;

// allocate integers [lb,ub) 
bvec_t bvec_mk(uint32_t lb, uint32_t ub);

uint32_t bvec_alloc(bvec_t *b);
void bvec_free(bvec_t *b, uint32_t x);

void bvec_print(const char *msg, bvec_t *b);
#endif
