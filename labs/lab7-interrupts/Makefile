all: build 

# install

build:
	make -C timer-int
	make -C gprof

clean:
	rm -f *~
	make -C timer-int clean
	make -C gprof clean
