

ARM = arm-none-eabi
CC = $(ARM)-gcc
LD  = $(ARM)-ld
AS  = $(ARM)-as
OD  = $(ARM)-objdump
OCP = $(ARM)-objcopy
LPI = $(LIBPI_PATH)/libpi.a

ifndef LIBPI_PATH
$(error LIBPI_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

CFLAGS = -Wall -nostdlib -nostartfiles -ffreestanding  -march=armv6 -O -std=gnu99 -I$(LIBPI_PATH) -I.
ASFLAGS = --warn --fatal-warnings -mcpu=arm1176jzf-s -march=armv6zk
