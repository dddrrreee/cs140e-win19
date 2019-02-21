#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "demand.h"
#include "pi-fs.h"

static int is_legal(perm_t p)  {
    if(p == perm_rd || p == perm_wr || p == perm_rw)
        return 1;
    return 0;
}

int has_perm(int x, perm_t p) {
    if(!is_legal(p))
        error("invalid permission: %x\n", p);
    return (x&p) == p;
}

/*
    this is broken since we do not track if file is opened in more restrictive
    mode.  doesn't matter for pi.

    "The  argument  flags  must  include  one of the following access modes:
    O_RDONLY, O_WRONLY, or O_RDWR."
*/
int flag_to_perm(int flags) {
    if((flags&O_WRONLY) == O_WRONLY)
        return perm_wr;
    if((flags&O_RDWR) == O_RDWR)
        return perm_wr | perm_rd;
    if((flags&O_RDONLY) == O_RDONLY)
        return perm_rd;
    error("invalid flag: 0x%x 0%o\n", flags, flags);
}

file_t *file_new(size_t hint) {
    if(hint < 8192)
        hint = 8192;
    hint *= 2;

    file_t *f = calloc(1,sizeof *f);
    f->n_data = 0;
    f->n_alloc = hint;

    f->data = calloc(1,hint);
    assert(f->data);
    return f;
}

void file_realloc(file_t *f, size_t needed) {
    assert(needed > 0);

    size_t n = needed * 2;
    char *p = calloc(1, n); // we zero fill so can ignore holes.

    memcpy(p, f->data, f->n_data);
    free(f->data);
    f->data = p;
    f->n_alloc = n;
}

dirent_t *ent_lookup(dirent_t *c, const char *path) {
    if(path[0] != '/')
        return 0;
    for(int i = 0; c[i].name; i++)
        if(strcmp(c[i].name, path) == 0) {
            // lazily allocate file structure.
            if(!c[i].f)
                c[i].f = file_new(8);
            return &c[i];
        }
    return 0;
}

dirent_t * file_lookup(int *retv, dirent_t *d, const char *path, int perm) {
    dirent_t *e = ent_lookup(d, path);
    *retv = 0;
    if(!e) {
        *retv = -ENOENT;
        return 0;
    }
    if(!has_perm(e->flags, perm)) {
        *retv = -EACCES;
        return 0;
    }
    return e;
}

