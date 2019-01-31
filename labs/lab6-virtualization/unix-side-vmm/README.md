This is the Unix side emulation (or virtualization) layer for the pi.
The big picture is that we override low-level functions in `libpi`
and and re-implement them on Unix.

Mechanically: you simply add pi files to the `Makefile`.  It will 
compile them on your laptop (the host) and link them against a
fake host-side implementation you will write in `virtual-rpi.c`.
