APP = blink
ARMGNU ?= arm-none-eabi

COPS = -Wall  -nostdlib -nostartfiles -ffreestanding -fno-builtin

gcc : kernel.img

all : gcc 

clean :
	rm -f *.o
	rm -f *.bin
	rm -f *.hex
	rm -f *.elf
	rm -f *.list
	rm -f *.img
	rm -f *.bc
	rm -f *.clang.s
	rm -f *~

OBJECTS = vectors.o $(APP).o
LIBRARY =

vectors.o : vectors.s
	$(ARMGNU)-as vectors.s -o vectors.o

%.o: %.c
	# $(ARMGNU)-gcc $(COPS) -O2 -c $< -o $@

	$(ARMGNU)-gcc $(COPS) -c $< -o $@

kernel.img : loader $(OBJECTS)
	$(ARMGNU)-ld $(OBJECTS) $(LIBRARY) -T loader -o $(APP).elf 
	$(ARMGNU)-objdump -D $(APP).elf > $(APP).list
	$(ARMGNU)-objcopy $(APP).elf -O ihex $(APP).hex
	$(ARMGNU)-objcopy $(APP).elf -O binary kernel.img


