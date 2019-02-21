#ifndef __PI_FS_H__
#define __PI_FS_H__
#include <stdlib.h>

struct dirent;
typedef int (*on_write_fp)(struct dirent *e, const char *path, const char *buf,
                            size_t size, off_t offset, void *data);

// 0 = skip write to base file.  1 = do it.
typedef int (*on_read_fp)(struct dirent *e, const char *path, char *buf,
                            size_t size, off_t offset, void *data);


// this way of creating a ramFS is not the right way to do things.
// we use expedient hacks so we can strip the things down for a 
// couple hour lab --- you'd really want a real tree (or perhaps DAG) 
// that mirrors the fake file system you are making.  unfortunately,
// if we gave that as starter code, you'd have to spend a bunch of 
// time figuring it out.  and if we didn't, you'd have to write it.
// i'd rather you spend time in lab on figuring out FUSE.
typedef struct pi_file {
    char *data;
    size_t n_alloc,    // total bytes allocated.
             n_data;     // how many bytes of data
} file_t;

// for first lab: no directories.
typedef struct dirent {
    char *name;
    int flags;
    file_t *f;

    // allow user to extend each file/dir with actions.
    on_write_fp on_wr;
    on_read_fp on_rd;
} dirent_t;

/**********************************************************************
 * unix permissions. for examples:
 *      - "man 2 stat"
 *      - "man 2 open"
 */
typedef enum {
    perm_rd = 0444,
    perm_wr = 0222,
    // not sure if we use this.
    perm_exec = 0111,
    perm_rw = perm_rd | perm_wr | perm_exec,
} perm_t;

// are <perm> compatible with <file_perm>?
int has_perm(int file_perm, perm_t request_perm);
// translate open flag permissions to our permissions.
int flag_to_perm(int flags);

// allocate a new file structure, initial file size =<hint>
file_t *file_new(size_t hint) ;
// reallocate the file with file stucture to at least <needed>
void file_realloc(file_t *f, size_t needed);

// lookup <path> in <dir>
dirent_t *ent_lookup(dirent_t *dir, const char *path);

// lookup a path in <d>: any error code is written to <retv>
dirent_t * file_lookup(int *retv, dirent_t *d, const char *path, int perm);

#endif
