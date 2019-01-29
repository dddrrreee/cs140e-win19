// engler: showing how to check if an fd is open
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

static int is_open(int fd) {
	return fcntl(fd, F_GETFL) >= 0;
}

int main(void) { 
	assert(is_open(0));
	assert(is_open(1));
	assert(is_open(2));
	assert(!is_open(3));
	printf("SUCCESS!\n");
	return 0;
}

