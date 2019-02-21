all: build 

# install

build:
	make -C part0-hello
	make -C part1-pi-fs
	make -C part2-redirection

clean:
	rm -f *~ *.bak
	make -C part0-hello clean
	make -C part1-pi-fs clean
	make -C part2-redirection clean
