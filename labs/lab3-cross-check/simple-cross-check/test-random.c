#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int main() { 
	long l = random(); 
	printf("%ld\n", l);
	assert(l == 1804289383);
}
