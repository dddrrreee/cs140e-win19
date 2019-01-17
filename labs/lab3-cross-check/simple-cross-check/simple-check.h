#ifndef __SIMPLE_CHECK_H__
#define __SIMPLE_CHECK_H__

typedef int (*gen_t)(unsigned *);

// hack to make new functions in C.
#define xc_lambda_gen(_body) ({        		\
        int __fn__(unsigned *out) _body              \
        __fn__;                                 \
})

// run function <int a(int)> using all values produced by g, print out all
// values of get32/put32.
void xc_run_fn_iu(const char *A, int (*a)(unsigned), gen_t g);

// run <int a(int)> once on <v> ("a(v)") print out all values of get32/put32.
void xc_run_fn_iu_once(const char *A, int (*a)(unsigned), unsigned v);

void xc_run_fn_vv_once(const char *A, void (*a)(void));

// returns a generator to spit out values for a gpio pin variable.
gen_t xc_get_pin_gen(void);

#endif

