#include <stdio.h>
#include <unistd.h>


#include "pi-vmm-ops.h"
#include "vmm.h"


int main(void) {
	fprintf(stderr, "waiting for ready\n");
	int v = get_uint();
	fprintf(stderr, "got %d\n", v);
	if(v != OP_READY)
		fprintf(stderr, "expected READY (%d), got %d\n", OP_READY, v);

	fprintf(stderr, "about to run fake pi\n");
	rpi_notmain();
	fprintf(stderr, "fake pi done\n");
	return 0;
}
