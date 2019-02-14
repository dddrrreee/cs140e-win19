all: build 

# install

build:
	make -C hello-fixed
	make -C shell-pi-side
	make -C shell-unix-side

clean:
	rm -f *~ *.bak
	make -C hello-fixed clean
	make -C shell-pi-side clean
	make -C shell-unix-side clean
