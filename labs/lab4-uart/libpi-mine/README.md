This is our gross hack for attempting to minimize git conflicts:

	1.  Do not change `Makefile` directly. 
	2. If you need to add or remove targets change `manifest.mk`.
	3. Do not ever modify any files in `libpi.support`.  If you want
	to replace them (e.g., by writing your own uart implementation),
	change the relevant variables in `manifest.mk`.

Apologies.  The trouble is some of these files are in flux, and if
we push changes later it's easy to run into conflicts.  If we instead
have you create private copies and manually move any late changes over,
it's easy to miss one, or make a mistake.
