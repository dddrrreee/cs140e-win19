NAME = pi-vmm

PI = $(LIBPI_PATH)
include $(PI)/includes.mk

ifndef NAME
$(error NAME is not set: this should be the name of the binary you want to build)
endif

SRC = $(NAME).c 
OBJS = $(SRC:.c=.o)

CFLAGS += -I$(PI) -I.

all : libpi $(NAME) # run

libpi:
	make -C $(PI)
	
$(NAME): $(PI)/memmap $(OBJS) $(PI)/libpi.a  $(PI)/cs140e-start.o
	arm-none-eabi-ld $(OBJS) $(PI)/cs140e-start.o $(PI)/libpi.a -T $(PI)/memmap -o $(NAME).elf
	arm-none-eabi-objdump -D $(NAME).elf > $(NAME).list
	arm-none-eabi-objcopy $(NAME).elf -O binary $(NAME).bin

run:
	my-install $(NAME).bin

clean :
	rm -f *.o *.bin *.elf *.list *.img *~ Makefile.bak

depend:
	makedepend -I$(PI)/ *.[ch] 

# DO NOT DELETE
