#ifndef __RPI_H__
#define __RPI_H__

// return *(volatile unsigned *)addr
unsigned (get32)(volatile void *addr);

// *(volatile unsigned *)addr = v
void (put32)(volatile void *addr, unsigned val);

#endif /* __RPI_H__ */
