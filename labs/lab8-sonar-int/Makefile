all: build 

# install

build:
	make -C sonar-hc-sr04
	make -C sonar-int

clean:
	rm -f *~ *.bak
	make -C sonar-hc-sr04 clean
	make -C sonar-int clean
