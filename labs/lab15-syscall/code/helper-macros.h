#ifndef __RPI_MACROS_H__
#define __RPI_MACROS_H__

#define is_aligned(x, a)        (((x) & ((typeof(x))(a) - 1)) == 0)

// check bitfield positions.
#define check_bitfield(T, field, off, nbits) do {                       \
        union _u {                                                      \
                T s;                                                    \
                unsigned u;                                             \
        } x = (union _u) { .s = (T){ .field = 1 } };                    \
        unsigned exp = 1<<off;                                          \
        if(x.u != exp)                                                  \
                panic("offset wrong: expect %x got %x %s [off=%d,nbits=%d]\n", \
                                          exp,x.u,#field,off,nbits);        \
        assert(x.u == exp);                                             \
                                                                        \
        unsigned shift = 32 - nbits;                                \
        unsigned set_all = (~0UL << shift) >> shift;                      \
        exp = set_all << off;                                           \
        x.s.field = ~0;                                                 \
        if(x.u != exp)                                                  \
                panic("width wrong: expect %x got %x %s [off=%d,nbits=%d]\n", \
                                          exp,x.u,#field,off,nbits);        \
        assert(x.u == exp);                                             \
} while(0)

#define print_field(x, field) do {                   \
        printk("\t0b%b\t= %s\n", (x)->field, #field);   \
        if(((x)->field) > 8)                            \
            printk("\t0x%x\n", (x)->field);             \
} while(0)

#endif
