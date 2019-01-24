all: build 

build:
	make -C libpi-mine
	make -C uart-tests
	make -C pi-cat

clean:
	rm -f *~
	make -C libpi-mine clean
	make -C uart-tests clean
	make -C pi-cat clean
