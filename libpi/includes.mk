
ARM = arm-none-eabi
CC = $(ARM)-gcc
LD  = $(ARM)-ld
AS  = $(ARM)-as
OD  = $(ARM)-objdump
OCP = $(ARM)-objcopy
LPI = $(LIBPI_PATH)/libpi.a
LPI_PIC = $(LIBPI_PATH)/libpi.PIC.a

ifndef LIBPI_PATH
$(error LIBPI_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

CFLAGS = -O -Wall -nostdlib -nostartfiles -ffreestanding  -march=armv6 -std=gnu99 -I$(LIBPI_PATH) -I.
ASFLAGS = --warn --fatal-warnings -mcpu=arm1176jzf-s -march=armv6zk

ifdef USE_PIC
override CFLAGS += \
	    -frecord-gcc-switches\
	    -gdwarf-2\
	    -fdata-sections -ffunction-sections\
	    -Wall\
	    -Wl,--warn-common\
	    -Wl,--gc-sections\
	    -Wl,--emit-relocs\
	    -fPIC\
	    -msingle-pic-base\
	    -mpic-register=r9\
	    -mno-pic-data-is-text-relative

TARGET = libpi.PIC.a
endif
