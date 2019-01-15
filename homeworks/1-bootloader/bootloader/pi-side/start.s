@ based on dwelch's start for bootloader.

.globl _start
_start:
    b skip
.space 0x200000-0x8004,0
skip:
    mov sp,#0x08000000
    bl notmain
hang: b reboot

.globl BRANCHTO
BRANCHTO:
    bx r0
