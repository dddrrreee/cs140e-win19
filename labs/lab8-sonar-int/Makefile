all: build 

# install

build:
	make -C sonar-hc-sr04
	make -C sonar-int
	make -C gpio-int

clean:
	rm -f *~ *.bak
	make -C sonar-hc-sr04 clean
	make -C sonar-int clean
	make -C gpio-int clean
