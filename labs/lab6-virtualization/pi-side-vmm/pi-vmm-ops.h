#ifndef _PI_VMM_OPS__
#define _PI_VMM_OPS__
enum { 
	/*
	 * send two 32-bit values: <OP_READ32>, <addr>.
	 * reply: one 32-bit value, <val>
 	 */
	OP_READ32 = 11,

	/*
	 * send three 32-bit values: <OP_READ32>, <addr>, <val>.
	 * no reply.
 	 */
	OP_WRITE32,

	/* single 32-bit value: reboot. */
	OP_DONE,

	/* single 32-bit value: any error. */
	OP_ERROR,

	OP_READY
};
#endif
