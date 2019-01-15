#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "demand.h"
#include "../shared-code/simple-boot.h"
#include "support.h"

// read entire file into buffer.  return it, write totat bytes to <size>
unsigned char *read_file(int *size, const char *name) {
	unimplemented();
	return 0;
}

#define _SVID_SOURCE
#include <dirent.h>
const char *ttyusb_prefixes[] = {
	"ttyUSB",	// linux
	"tty.SLAB_USB", // mac os
	0
};

// open the TTY-usb device:
//	- use <scandir> to find a device with a prefix given by ttyusb_prefixes
//	- returns an open fd to it
// 	- write the absolute path into <pathname> if it wasn't already
//	  given.
int open_tty(const char **portname) {
	unimplemented();
	return -1;
}
