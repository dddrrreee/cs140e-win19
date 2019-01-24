CC= arm-none-eabi-gcc

PI_LIB_PATH = ../libpi-mine/
PI_INC_PATH = ../libpi-include
PI_LIB = $(PI_LIB_PATH)/libpi.a
PI_DEPS = $(PI_LIB_PATH)/memmap $(PI_LIB_PATH)/start.o $(PI_LIB)

CFLAGS = -I$(PI_INC_PATH) -I. -Wall -Og -nostdlib -nostartfiles -ffreestanding 

SRC = hello-once.c char-inf.c
OBJS = $(SRC:.c=.o)

all : $(PI_LIB) 2hello-once 0char-inf 1hello-inf

$(PI_LIB):
	make -C $(PI_LIB_PATH)

2hello-once: $(PI_DEPS) 2hello-once.o
	arm-none-eabi-ld $(PI_LIB_PATH)/start.o 2hello-once.o $(PI_LIB) -T $(PI_LIB_PATH)/memmap -o 2hello-once.elf
	arm-none-eabi-objdump -D 2hello-once.elf > 2hello-once.list
	arm-none-eabi-objcopy 2hello-once.elf -O binary 2hello-once.bin

0char-inf: $(PI_DEPS) 0char-inf.o
	arm-none-eabi-ld $(PI_LIB_PATH)/start.o 0char-inf.o $(PI_LIB) -T $(PI_LIB_PATH)/memmap -o 0char-inf.elf
	arm-none-eabi-objdump -D 0char-inf.elf > 0char-inf.list
	arm-none-eabi-objcopy 0char-inf.elf -O binary 0char-inf.bin

1hello-inf: $(PI_DEPS) 1hello-inf.o
	arm-none-eabi-ld $(PI_LIB_PATH)/start.o 1hello-inf.o $(PI_LIB) -T $(PI_LIB_PATH)/memmap -o 1hello-inf.elf
	arm-none-eabi-objdump -D 1hello-inf.elf > 1hello-inf.list
	arm-none-eabi-objcopy 1hello-inf.elf -O binary 1hello-inf.bin

clean :
	rm -f *.o *.bin *.elf *.list *.img *~ Makefile.bak

depend:
	makedepend -I$(LIBPI)/.. *.[ch] 

# DO NOT DELETE
