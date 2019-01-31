all: build 

# install

build:
	make -C unix-side-vmm
	make -C pi-side-vmm
	make -C blink
	make -C hello

clean:
	rm -f *~
	make -C blink clean
	make -C unix-side-vmm clean
	make -C pi-side-vmm clean
	make -C hello clean
