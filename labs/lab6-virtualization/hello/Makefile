all: build 


build:
	make -f Makefile.pi
	make -f Makefile.fake

run:
	make -f Makefile.pi run
	sleep 1;
	make -f Makefile.fake run

clean:
	rm -f *~
	make -f Makefile.pi clean
	make -f Makefile.fake clean
