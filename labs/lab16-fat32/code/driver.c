/* simple fat32 read-only fs. */
#include "rpi.h"
#include "fat32.h"
#include "pi-fs.h"
#include "helper-macros.h"

// allocate <nsec> worth of space, read in from SD card, return pointer.
void *sec_read(uint32_t lba, uint32_t nsec) {
    void *data = 0;
    unimplemented();
    return data;
}

// there are other tags right?
static int is_fat32(int t) { return t == 0xb || t == 0xc; }

typedef struct fat32 {
    uint32_t fat_begin_lba,
            cluster_begin_lba,
            sectors_per_cluster,
            root_dir_first_cluster;
    uint32_t *fat;
    uint32_t n_fat;
} fat32_fs_t;

/*
    Field                       Microsoft's Name    Offset   Size        Value
    Bytes Per Sector            BPB_BytsPerSec      0x0B(11) 16 Bits     Always 512 Bytes
    Sectors Per Cluster         BPB_SecPerClus      0x0D(13) 8 Bits      1,2,4,8,16,32,64,128
    Number of Reserved Sectors  BPB_RsvdSecCnt      0x0E(14) 16 Bits     Usually 0x20
    Number of FATs              BPB_NumFATs         0x10(16) 8 Bits      Always 2
    Sectors Per FAT             BPB_FATSz32         0x24(36) 32 Bits     Depends on disk size
    Root Directory First ClusterBPB_RootClus        0x2C(44) 32 Bits     Usually 0x00000002
    Signature                   (none)              0x1FE(510)16 Bits     Always 0xAA55
*/
static struct fat32 fat32_mk(uint32_t lba_start, fat32_boot_sec_t *b) {
    struct fat32 fs;

    fs.fat_begin_lba = lba_start + b->reserved_area_nsec;
    printk("begin lba = %d\n", fs.fat_begin_lba);
    unimplemented();
    return fs;
}

// given cluster_number get lba
uint32_t cluster_to_lba(struct fat32 *f, uint32_t cluster_num) {
    unimplemented();
}

int fat32_dir_cnt_entries(fat32_dir_t *d, int n) {
    int cnt = 0;
    for(int i = 0; i < n; i++, d++)
        if(!fat32_dirent_free(d) && d->attr != FAT32_LONG_FILE_NAME)
                cnt++;
    return cnt;
}

// translate fat32 directories to dirents.
pi_dir_t fat32_to_dirent(fat32_fs_t *fs, fat32_dir_t *d, uint32_t n) {
    pi_dir_t p;
    p.n = fat32_dir_cnt_entries(d,n);
    p.dirs = kmalloc(p.n * sizeof *p.dirs); 

    unimplemented();
    return p;
}

// read in an entire file.  note: make sure you test both on short files (one cluster)
// and larger ones (more than one cluster).
pi_file_t fat32_read_file(fat32_fs_t *fs, dirent_t *d) {
    // the first cluster.
    uint32_t id = d->cluster_id;
    pi_file_t f;
    
    // compute how many clusters in the file.
    //      note: you know you are at the end of a file when 
    //          fat32_fat_entry_type(fat[id]) == LAST_CLUSTER.
    // compute how many sectors in cluster.
    // allocate this many bytes.
    unimplemented();

    return f;
}

dirent_t *dir_lookup(pi_dir_t *d, char *name) {
    for(int i = 0; i < d->n; i++) {
        dirent_t *e = &d->dirs[i];
        if(strcmp(name, e->name) == 0)
            return e;
    }
    return 0;
}

void notmain() {
    uart_init();

    // Part1
    //    1. adapt bzt's sd card driver.
    //    2. write: pi_sd_init(), pi_sd_read,  sec_read.

#ifdef PART2
    // PART2:
    //    1. define the master boot record.
    //    2. read in.
    //    3. pull out the partition and check it.
    // in particular, write the structures in fat32.h so that the following
    // code works and compiles:

    struct mbr *mbr = sec_read(0, 1);
    fat32_mbr_check(mbr);

    struct partition_entry p;
    memcpy(&p, mbr->part_tab1, sizeof p);
    fat32_partition_print("partition 1", &p);

    // fat32
    assert(is_fat32(p.part_type));
    assert(!fat32_partition_empty((uint8_t*)mbr->part_tab1));
    assert(fat32_partition_empty((uint8_t*)mbr->part_tab2));
    assert(fat32_partition_empty((uint8_t*)mbr->part_tab3));
    assert(fat32_partition_empty((uint8_t*)mbr->part_tab4));
#endif

#ifdef PART3
    /*
        https://www.pjrc.com/tech/8051/ide/fat32.html
        The first step to reading the FAT32 filesystem is the read its
        first sector, called the Volume ID. The Volume ID is read using
        the LBA Begin address found from the partition table. From this
        sector, you will extract information that tells you everything
        you need to know about the physical layout of the FAT32 filesystem

        this is our <fat32_boot_sec_t>
    */

    fat32_boot_sec_t *b = 0;
    unimplemented();

    fat32_volume_id_check(b);
    fat32_volume_id_print("boot sector", b);

    // bonus: read in the file info structure
    struct fsinfo *info = sec_read(p.lba_start+1, 1);
    fat32_fsinfo_check(info);
    fat32_fsinfo_print("info struct", info);

    /* 
        The disk is divided into clusters. The number of sectors per
        cluster is given in the boot sector byte 13. <sec_per_cluster>

        The File Allocation Table has one entry per cluster. This entry
        uses 12, 16 or 28 bits for FAT12, FAT16 and FAT32.
    */

    // implement this routine.
    struct fat32 fs = fat32_mk(p.lba_start, b);

    // read in the both copies of the FAT.

    uint32_t *fat = 0, *fat2 = 0;
    unimplemented();

    // check that they are the same.
    assert(memcmp(fat, fat2, n_bytes) == 0);
#endif

#ifdef PART4
    int type = fat32_fat_entry_type(fat[2]);
    printk("fat[2] = %x, type=%s\n", fat[2], fat32_fat_entry_type_str(type));

    printk("lba.start=%d\n", p.lba_start);
    printk("cluster 2 to lba = %d\n", cluster_to_lba(&fs, 2));
    unsigned dir_lba = cluster_to_lba(&fs, b->first_cluster);
    printk("rood dir first cluster = %d\n", dir_lba);


    // calculate the number of directory entries.
    uint32_t dir_n = 0;
    unimplemented();

    // read in the directories.
    uint32_t dir_nsecs = 0;
    fat32_dir_t *dirs = 0;
    unimplemented();

    // this should just work.
    for(int i = 0; i < dir_n; ) {
        if(fat32_dirent_free(dirs+i)) {
            i++;
        } else {
            printk("dir %d is not free!:", i);
            i += fat32_dir_print("", dirs+i, dir_n - i);
        } 
    }

    pi_dir_t pdir = fat32_to_dirent(&fs, dirs, dir_n);

    // print out the contents of the directory.  this should make sense!
    for(int i = 0; i < pdir.n; i++) {
        dirent_t *e = &pdir.dirs[i];
        printk("\t%s\t\t->\tcluster id=%d, type=%s, nbytes=%d\n", 
                    e->name, e->cluster_id, e->is_dir_p?"dir":"file", e->nbytes);
    }
#endif

#ifdef PART5
    // this should succeed, and print the contents of config.
    dirent_t *e =dir_lookup(&pdir, "config.txt");
    assert(e);
    printk("FOUND: \t%s\t\t->\tcluster id=%x, type=%s, nbytes=%d\n", 
            e->name, e->cluster_id, e->is_dir_p?"dir":"file", e->nbytes);

    printk("config.txt:\n");
    printk("---------------------------------------------------------\n");
    pi_file_t f = fat32_read_file(&fs, e);
    for(int i = 0; i < f.n_data; i++)
        printk("%c", f.data[i]);
    printk("---------------------------------------------------------\n");
#endif

    clean_reboot();
}
