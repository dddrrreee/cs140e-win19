#ifndef __PI_SHELL_H__
#define __PI_SHELL_H__

/*
 * long term: we need to allocate space to run processes.  in this lab
 * we are just doing a quick/dirty approach and statically linking
 * applications to a fixed address (we will use dynamic linking later).
 *
 * for the code we've written thus far, the highest address has been
 * the interrupt stack (at 0x90000000), so we allow user programs to 
 * run a bove that with a max range somewhere around the 370MB range.
 * the pi has 512MB, but it appears the GPU takes some of that (i am
 * not sure how much!   need to find out.)  so we are conservative
 * in how big we allow things to get.
 */
#define LAST_USED_ADDRESSES 0x9000000
#define MAX_ADDRESS        0x16000000

// uses a stripped down protocol to get code from unix side, copy it
// into memory, jump to it.
int load_code(void);

unsigned get_uint(void);
void put_uint(unsigned u);

#endif
