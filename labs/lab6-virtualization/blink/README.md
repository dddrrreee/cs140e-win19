Simple example of compiling pi program both for pi (`Makefile.pi`) and
for unix (`Makefile.unix`).

   1.  `make` will build both.  `hello.bin` can run on the pi, `hello.unix`
   can run on unix and outsource GPIO writes to the pi.

   2. `make run` will run both.

   3. If it doesn't work, make sure you modified your bootloader to handle
   the `-exec` flag.
