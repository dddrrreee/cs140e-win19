###########################################################################
# To add your my-uart.c
#	1. Add it to the SRC variable:
#		SRC = my-uart.c
#	2. Delete SUPPORT_OBJS
#		SUPPORT_OBJS = 

# put all of your source files you have here. e.g., 
# 	SRC = my-uart.c
SRC = my-uart.c

###########################################################################
# these are .o's we give you --- when to use your my-uart.c you remove this.
# or define an empty target:

# when your gpio works, comment these out too.
SUPPORT_OBJS=	libpi.support/gpio.o libpi.support/gpioextra.o 
# libpi.support/uart.o

###########################################################################
# these are any .c files you want out of libpi.support/
# for hw1 you'll probably just leave these.  You can also rewrite any
# by removing them from here, and adding the path to your new file to
# SRC
SUPPORT_SRC = libpi.support/reboot.c 	\
		libpi.support/timer.c 	\
		libpi.support/helpers.s \
		libpi.support/mem-barrier.s 		\
		libpi.support/printf.c
