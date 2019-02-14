all: build 
# install

ADDR = 0x100000f0
# ADDR = 0x1000f0

build:
	# make -f Makefile.fixed LINK_ADDR=0x10000000
	# make -f Makefile.fixed LINK_ADDR=0x6000000
	make -f Makefile.fixed LINK_ADDR=$(ADDR)
	cp hello-fixed.$(ADDR).bin ../shell-unix-side/hello.bin

clean:
	make -f Makefile.fixed clean
