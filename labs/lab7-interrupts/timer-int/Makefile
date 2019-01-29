NAME = timer
CC= arm-none-eabi-gcc
AS = arm-none-eabi-as
SRC = $(NAME).c 

# CFLAGS =  -I$(LIBPI) -I. -std=gnu99 -Wall -Og -nostdlib -nostartfiles -ffreestanding
CFLAGS =  -I$(LIBPI) -I. -Wall -Og -std=gnu99 -nostdlib -nostartfiles -ffreestanding $(FPFLAGS)

# CFLAGS =  -I$(LIBPI) -I. -Wall -Og -std=gnu99 -Wall -Og -std=gnu99  -nostdlib -nostartfiles -ffreestanding  -mapcs-frame -fno-omit-frame-pointer -ffunction-sections -fdata-sections  

# these are flags for floating-point
# FPFLAGS = -mfloat-abi=hard   -march=armv6 -mfpu=vfp 
# START =  $(LIBPI)/fp-start.o
# LPI = $(LIBPI)/libpi-fp.a 

# non-float
LPATH= ../../lab4-uart/libpi-mine/
LPI_INC = $(LPATH)/../libpi-include
LPI = $(LPATH)/libpi.a 

LIBPI = $(LPI_INC) 
OBJS = $(SRC:.c=.o)  interrupts-asm.o interrupts-c.o cstart.o  timer-interrupt.o


ASFLAGS = $(FPFLAGS)

all : libpi kernel.img  run

libpi:
	make -C $(LPATH)

kernel.img : memmap start.o $(OBJS)
	arm-none-eabi-ld start.o $(OBJS) -T memmap -o $(NAME).elf  $(LPI)
	arm-none-eabi-objdump -D $(NAME).elf > $(NAME).list
	arm-none-eabi-objcopy $(NAME).elf -O binary kernel.img

run:
	my-install kernel.img

clean :
	rm -f *.o *.bin *.elf *.list *.img *~ Makefile.bak

ctags:
	ctags *.[ch] ../../libpi/*.[csh]
etags:
	etags *.[ch] ../../libpi/*.[csh]

depend:
	makedepend -I$(LIBPI)/ *.[ch] 

