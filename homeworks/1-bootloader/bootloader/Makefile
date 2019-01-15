# you need to change this to where your sd card goes.
SD_PATH = /media/engler/0403-0201/

ifndef SD_PATH
$(error SD_PATH is not set: give absolute path to where your sd card is mounted)
endif

all: build 

build:
	make -C pi-side
	make -C unix-side

clean:
	make -C pi-side clean
	make -C unix-side clean
	rm -f *~
	
install:
	cp pi-side/kernel.img $(SD_PATH)/kernel.img
	sync
