// To keep this in the first portion of the binary.
.section ".text.boot"

.globl _start
_start:
    mov r0, #0b11111
    msr cpsr_c, r0

    // dwelch starts here: mov sp, #0x8000
    mov sp, #0x8000
    mov fp, #0  // i don't think necessary.
    @ bl notmain
    bl _cstart
    bl rpi_reboot // if they return just reboot.
