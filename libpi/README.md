You should make your own libpi (e.g., `my-libpi`) and:
  - copy the `Makefile` from this directory there.
  - start putting files from your labs there.
  - copy any other files from this directory you need there too.

It should be your own stand-alone `libpi.a`, where you start replacing our 
code, and rewriting / restructuring everything so it's clean.


The idea:
  - When you make a useful file in a lab, put it there so you can 
  easily use in other labs.

  -  Don't change our header files, since those are the common interfaces.

  - Modify the makefile to use your new files and/or replace our
  versions (which are always prefixed with "cs140e-").

  - includes.mk defines some helpful makefile macros if you want to 
  include them in your various lab makefiles.


If you get a conflict and want to discard your local changes:
 [`git checkout -- <file>`](https://docs.gitlab.com/ee/topics/git/numerous_undo_possibilities_in_git/)
