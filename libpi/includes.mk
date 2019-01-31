ARM = arm-none-eabi
CC = $(ARM)-gcc
LD  = $(ARM)-ld
AS  = $(ARM)-as
OD  = $(ARM)-objdump
OCP = $(ARM)-objcopy
CFLAGS = -Wall -nostdlib -nostartfiles -ffreestanding  -march=armv6 -Og -std=gnu99 -I$(LIBPI_PATH) -I.
ASFLAGS = --warn --fatal-warnings 
# -mcpu=arm1176jzf-s -march=armv6zk

