What to do:

  1. Implement redir() in redirect.c
  2. You should pass `make run`: this tests that you can (1) read `stdout`
  and `stderr` from a subprocess and (2) your code exits when it does rather
  than hanging.
  3. `make run.fake-pi`: this checks that you correctly listen for shell 
  prompts.
  4. `make run.pi`: checks that you can control your pi-shell.
