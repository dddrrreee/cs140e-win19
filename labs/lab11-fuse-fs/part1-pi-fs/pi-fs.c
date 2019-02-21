/* 
 * part1: trivial file system that we are going to repurpose for the pi.  
 * 
 * useful helper routines in pi-fs.h/pi-fs-support.c
 *
 * implement anything with unimplemented();
 */

#define FUSE_USE_VERSION 26
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <assert.h>
#include "demand.h"

#include "pi-fs.h"

static int do_reboot(dirent_t *e, const char *path, const char *buf,
                            size_t size, off_t offset, void *data);
static int do_echo(dirent_t *e, const char *path, const char *buf,
                            size_t size, off_t offset, void *data);
static int do_run(dirent_t *e, const char *path, const char *buf,
                            size_t size, off_t offset, void *data);


// simple pi file system: no directories.
static dirent_t root[] = {
    { .name = "/echo.cmd", perm_rw, 0, .on_wr = do_echo },
    { .name = "/reboot.cmd", perm_rw, 0, .on_wr = do_reboot},
    { .name = "/run.cmd", perm_rw, 0, .on_wr = do_run},
    { .name = "/console", perm_rd, 0 },
    { 0 }
};

static int pi_open(const char *path, struct fuse_file_info *fi) {
    note("opening\n");
    unimplemented();
}

static int pi_readdir(const char *path, void *buf, fuse_fill_dir_t filler, 
                            off_t offset, struct fuse_file_info *fi) {
    note("readdir\n");
    if(strcmp(path, "/") != 0)
        return -ENOENT;

    // no sub-directories
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    unimplemented();
    return 0;
}

static int pi_getattr(const char *path, struct stat *stbuf) {
    memset(stbuf, 0, sizeof(struct stat));
    if(strcmp(path,"/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        // we need one link for each subdir since they point to us.
        // plus 2 for "." and parent (or ".." in the case of "/").
        stbuf->st_nlink = 2;
        return 0;
    }

    // regular file.
    dirent_t *e = ent_lookup(root, path);
    if(!e)
        return -ENOENT;
    unimplemented();
    return 0;
}

static int pi_read(const char *path, char *buf, size_t size, 
                    off_t offset, struct fuse_file_info *fi) {

    note("****** READ\n");
    int retv;
    dirent_t *e = file_lookup(&retv, root, path, perm_rd);
    if(!e)
        return retv;

    unimplemented();
    return size;
}

static int pi_write(const char *path, const char *buf, size_t size, 
        off_t offset, struct fuse_file_info *fi) {

    note("****** WRITE:<%s> off=%ld, size=%ld\n", path,offset,size);
    int retv;
    dirent_t *e = file_lookup(&retv, root, path, perm_wr);
    if(!e)
        return retv;
    unimplemented();
    return size;
}

/** Change the size of a file */
static int pi_truncate(const char *path, off_t length) {
    int retv;
    dirent_t *e = file_lookup(&retv, root, path, perm_wr);
    if(!e)
        return retv;
    
    unimplemented();
    return 0;
}

static int 
pi_ftruncate(const char *path, off_t offset, struct fuse_file_info *fi) {
    return pi_truncate(path, offset);
}

/* these were enough for me for the lab. */
static struct fuse_operations pi_oper = {
    .getattr = pi_getattr, 
    .readdir = pi_readdir,
    .open = pi_open, 
    .read = pi_read,
    .write = pi_write,
    .truncate = pi_truncate,
    .ftruncate = pi_ftruncate,
};

/*
    -d Enable debugging output (implies -f).

    -f Run in foreground; this is useful if you're running under a
    debugger. WARNING: When -f is given, Fuse's working directory is
    the directory you were in when you started it. Without -f, Fuse
    changes directories to "/". This will screw you up if you use
    relative pathnames.

    -s Run single-threaded instead of multi-threaded. 
*/
int main(int argc, char *argv[]) {
    assert(ent_lookup(root, "/console"));
    assert(ent_lookup(root, "/echo.cmd"));
    assert(ent_lookup(root, "/reboot.cmd"));
    assert(ent_lookup(root, "/run.cmd"));
    return fuse_main(argc, argv, &pi_oper, 0);
}

// not needed for this part.
static int do_reboot(dirent_t *e, const char *path, const char *buf,
                            size_t size, off_t offset, void *data) {
    error("dont need for part1\n");
}

static int do_echo(dirent_t *e, const char *path, const char *buf,
                            size_t size, off_t offset, void *data) {
    error("dont need for part1\n");
}

static int do_run(dirent_t *e, const char *path, const char *buf,
                            size_t size, off_t offset, void *data) {
    error("dont need for part1\n");
} 
