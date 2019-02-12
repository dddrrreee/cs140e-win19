@empty stub routines.  use these, or make your own.

.globl test_csave
test_csave:
	bx lr

.globl test_csave_stmfd
test_csave_stmfd:
	bx lr

.globl rpi_get_cpsr
rpi_get_cpsr:
	bx lr

.globl rpi_cswitch
rpi_cswitch:
	bx lr

@ [Make sure you can answer: why do we need to do this?]
@
@ use this to setup each thread for the first time.
@ setup the stack so that when cswitch runs it will:
@	- load address of <rpi_init_trampoline> into LR
@	- <code> into r1, 
@	- <arg> into r0
@ 
.globl rpi_init_trampoline
rpi_init_trampoline:
	bx lr
