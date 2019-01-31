/*
 * General functions we use.  These could be broken into multiple small
 * header files, but that's kind of annoying to context-switch through,
 * so we put all the main ones here.
 * 
 * In order to run on Unix, I think we have to be more thorough with
 * NAME() uses.
 */
#ifndef __RPI_H__
#define __RPI_H__

/*****************************************************************************
 * pi types
 */

#ifndef FAKE_PI
	// if we are compiling fake, then I think we want to override this.
	typedef unsigned uint32_t;
	typedef unsigned short uint16_t;
	typedef unsigned char uint8_t;
	typedef short int16_t;
	typedef int int32_t;
	typedef signed char int8_t;
	typedef unsigned size_t;
#	define offsetof(st, m) __builtin_offsetof(st, m)
#else
#	include <stddef.h>
#endif

#ifdef FAKE_PI_IMPL
#	define NAME(x) (rpi_ ## x)
#else
#	define NAME(x) x
#endif

int printf(const char *format, ...);
int snprintf(char *buf, size_t n, const char *fmt, ...);
int puts(const char *msg);
int putchar(int c);


/*****************************************************************************
 * common device functions
 */
// uart functions
void NAME(uart_init)(void);
int NAME(uart_getc)(void);
void NAME(uart_putc)(unsigned c);

// simple timer functions.
void delay(unsigned ticks) ;
unsigned timer_get_time(void) ;
void delay_us(unsigned us) ;
void delay_ms(unsigned ms) ;


/*****************************************************************************
 * standard libc like functions for the pi.
 */
int printf(const char *format, ...);
int snprintf(char *buf, size_t n, const char *fmt, ...);
int puts(const char *msg);
int putchar(int c);

int strcmp(const char *_p, const char *q);
void *memset(void *_p, int c, size_t n) ;
int memcmp(const void *_s1, const void *_s2, size_t nbytes);
void *memcpy(void *dst, const void *src, size_t nbytes);

// reboot the pi.
void NAME(reboot)(void);

// reboot after printing out a string to cause the unix my-install to shut down.
void NAME(clean_reboot)(void);

// set pc value to <addr>
void BRANCHTO( unsigned addr);

// a no-op routine called to defeat the compiler.
void dummy(unsigned);

/*****************************************************************************
 * memory barriers
 */
void mb(void);
void dmb(void);
void dsb(void);

// use this if you need memory barriers.
void dev_barrier(void);


/*****************************************************************************
 * put/get from memory in a way that we can override easily + defeat compiler.
 */
// *(unsigned *)addr = v;
void NAME(PUT32)(unsigned addr, unsigned v);
void NAME(put32)(volatile void *addr, unsigned v);

// *(unsigned char *)addr = v;
void NAME(PUT16)(unsigned addr, unsigned short v);
void NAME(put16)(volatile void *addr, unsigned short v);

// *(unsigned char *)addr = v;
void NAME(PUT8)(unsigned addr, unsigned char v);
void NAME(put8)(volatile void *addr, unsigned char v);

// *(unsigned *)addr
unsigned GET32(unsigned addr);
unsigned get32(const volatile void *addr);

// *(unsigned short *)addr
unsigned short GET16(unsigned addr);
unsigned short get16(const volatile void *addr);

// *(unsigned char *)addr
unsigned char GET8(unsigned addr);
unsigned char get8(const volatile void *addr);

#include "gpio.h"

#endif
