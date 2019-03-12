#include "rpi.h"
#include "fat32.h"

/*************************************************************************************
 * helpers for master boot record
 */

void fat32_mbr_check(struct mbr *mbr) {
    assert(mbr->sigval == 0xAA55);
}

void fat32_partition_print(const char *msg, struct partition_entry *p) {
    printk("%s:\n", msg);
    printk("\tbootable  = %s\n", p->bootable_p ? "true" : "false");
    printk("\tchs_start = %x\n", p->chs_start);
    printk("\tpart type = %x (%s)\n", p->part_type, fat32_part_str(p->part_type));
    printk("\tchs_end   = %x\n", p->chs_end);
    printk("\tlba_start = %x\n", p->lba_start);
    printk("\tnector=   = %d (%dGB)\n", p->nsec, p->nsec / (2*1024*1024));
    assert(p->part_type == 0xb || p->part_type == 0xc);
}

// partition in master boot record.
const char *fat32_part_str(int x) {
#   define T(v, type) case v : return _XSTRING(type) ;
    switch(x) {
    T(0x00,   Empty)
    T(0x01,   FAT12 - CHS)
    T(0x04,   FAT16 - 16-32 MB - CHS)
    T(0x05,   Microsoft Extended - CHS)
    T(0x06,   FAT16 - 32 MB-2 GB - CHS)
    T(0x07,   NTFS)
    T(0x0b,   FAT32 - CHS)
    T(0x0c,   FAT32 - LBA)
    T(0x0e,   FAT16 - 32 MB-2 GB - LBA)
    T(0x0f,   Microsoft Extended - LBA)
    T(0x11,   Hidden FAT12 - CHS)
    T(0x14,   Hidden FAT16 - 16-32 MB - CHS)
    T(0x16,   Hidden FAT16 - 32 MB-2 GB - CHS)
    T(0x1b,   Hidden FAT32 - CHS)
    T(0x1c,   Hidden FAT32 - LBA)
    T(0x1e,   Hidden FAT16 - 32 MB-2 GB - LBA)
    T(0x42,   Microsoft MBR - Dynamic Disk)
    T(0x82,   Solaris x86 or Linux swap?)
    T(0x83,   Linux)
    T(0x84,   Hibernation)
    T(0x85,   Linux Extended)
    T(0x86,   NTFS Volume Set)
    T(0x87,   NTFS Volume Set)
    T(0xa0,   Hibernation)
    T(0xa1,   Hibernation)
    T(0xa5,   FreeBSD)
    T(0xa6,   OpenBSD)
    T(0xa8,   Mac OSX)
    T(0xa9,   NetBSD)
    T(0xab,   Mac OSX Boot)
    T(0xb7,   BSDI)
    T(0xb8,   BSDI swap)
    T(0xee,   EFI GPT Disk)
    T(0xef,   EFI System Partition)
    T(0xfb,   Vmware File System)
    T(0xfc,   Vmware swap)
    default: panic("unknown type: %x\n", x);
    }
}

int fat32_partition_empty(uint8_t *part) {
    for(int i = 0; i < 16; i++)
        if(part[i] != 0)
            return 0;
    return 1;
}

/******************************************************************************
 * boot record helpers.
 */
#define is_pow2(x) (((x) & -(x)) == (x))

#include "helper-macros.h"

static int is_fat32(int t) { return t == 0xb; }

void fat32_volume_id_check(fat32_boot_sec_t *b) {
    // has to be a multiple of 512 or the sd.c driver won't really work.
    // currently assume 512
    assert(b->bytes_per_sec == 512);
    assert(b->nfats == 2);
    assert(b->sig == 0xAA55);

    assert(is_pow2(b->sec_per_cluster));
    unsigned n = b->bytes_per_sec;
    assert(n == 512 || n == 1024 || n == 2048 || n == 4096);
    assert(!b->max_files);
    // 0 if size does not fit in 2bytes, which should always be true.
    assert(b->fs_nsec == 0);
    // removable disk.
    // assert(b->media_type == 0xf0);
    assert(b->zero == 0);
    assert(b->nsec_in_fs != 0);

    // usually these apparently.
    assert(b->info_sec_num == 1);
    assert(b->backup_boot_loc == 6);
    assert(b->extended_sig == 0x29);

    check_off(fat32_boot_sec_t, oem,                 3,      8);
    check_off(fat32_boot_sec_t, bytes_per_sec,       11,      2);
    check_off(fat32_boot_sec_t, sec_per_cluster,     13,      1);
    check_off(fat32_boot_sec_t, reserved_area_nsec,  14,      2);
    check_off(fat32_boot_sec_t, nfats,               16,      1);
    check_off(fat32_boot_sec_t, max_files,           17,      2);
    check_off(fat32_boot_sec_t, fs_nsec,             19,      2);
    check_off(fat32_boot_sec_t, media_type,          21,      1);
    check_off(fat32_boot_sec_t, sec_per_track,       24,      2);
    check_off(fat32_boot_sec_t, n_heads,             26,      2);
    check_off(fat32_boot_sec_t, hidden_secs,         28,      4);
    check_off(fat32_boot_sec_t, nsec_in_fs,          32,      4);
    check_off(fat32_boot_sec_t, nsec_per_fat,         36,      4);
    check_off(fat32_boot_sec_t, mirror_flags,         40,      2);
    check_off(fat32_boot_sec_t, version,             42,      2);
    check_off(fat32_boot_sec_t, first_cluster,       44,      4);
    check_off(fat32_boot_sec_t, info_sec_num,        48,      2);
    check_off(fat32_boot_sec_t, backup_boot_loc,     50,      2);
    check_off(fat32_boot_sec_t, reserved,            52,      12);
    check_off(fat32_boot_sec_t, logical_drive_num,   64,      1);
    check_off(fat32_boot_sec_t, extended_sig,        66,      1);
    check_off(fat32_boot_sec_t, serial_num,          67,      4);
    check_off(fat32_boot_sec_t, volume_label,        71,      11);
    check_off(fat32_boot_sec_t, fs_type,             82,      8);
    check_off(fat32_boot_sec_t, sig,                 510,     2);
}

void fat32_volume_id_print(const char *msg, fat32_boot_sec_t *b) {
    printk("%s:\n", msg);
    char s[9];
    memcpy(s, b->oem, 8);
    s[8] = 0;
    printk("\toem               = <%s>\n", s);
    printk("\tbytes_per_sec     = %d\n", b->bytes_per_sec);
    printk("\tsec_per_cluster   = %d\n", b->sec_per_cluster);
    printk("\treserved size     = %d\n", b->reserved_area_nsec);
    printk("\tnfats             = %d\n", b->nfats);
    printk("\tmax_files         = %d\n", b->max_files);
    printk("\tfs n sectors      = %d\n", b->fs_nsec);
    printk("\tmedia type        = %x\n", b->media_type);
    printk("\tsec per track     = %d\n", b->sec_per_track);
    printk("\tn heads           = %d\n", b->n_heads);
    printk("\tn hidden secs     = %d\n", b->hidden_secs);
    printk("\tn nsec in FS      = %d\n", b->nsec_in_fs);
    printk("\tn nsec per fat    = %d\n", b->nsec_per_fat);
    printk("\tn mirror flags    = %b\n", b->mirror_flags);
    printk("\tn version         = %d\n", b->version);
    printk("\tn first_cluster   = %d\n", b->first_cluster);
    printk("\tn info_sec_num    = %d\n", b->info_sec_num);
    printk("\tn back_boot_loc   = %d\n", b->backup_boot_loc);
    printk("\tn logical_drive_num= %d\n", b->logical_drive_num);
    printk("\tn extended sig    = %x\n", b->extended_sig);
    printk("\tn serial_num      = %x\n", b->serial_num);
    printk("\tn volume label    = <%s>\n", b->volume_label);
    printk("\tn fs_type         = <%s>\n", b->fs_type);
    printk("\tn sig             = %x\n", b->sig);
    fat32_volume_id_check(b);
}

/****************************************************************************************
 * fsinfo helpers.
 */
void fat32_fsinfo_print(const char *msg, struct fsinfo *f) {
    printk("%s:\n", msg);
    printk("\tsig1              = %x\n", f->sig1);
    printk("\tsig2              = %x\n", f->sig2);
    printk("\tsig3              = %x\n", f->sig3);
    printk("\tfree cluster cnt  = %d\n", f->free_cluster_count);
    printk("\tnext free cluster = %x\n", f->next_free_cluster);
}

void fat32_fsinfo_check(struct fsinfo *info) {
    assert(info->sig1 ==  0x41615252);
    assert(info->sig2 ==  0x61417272);
    assert(info->sig3 ==  0xaa550000);
}


/******************************************************************************
 * FAT table helpers.
 */
const char * fat32_fat_entry_type_str(uint32_t x) {
    switch(x) {
    case FREE_CLUSTER: return "FREE_CLUSTER";
    case RESERVED_CLUSTER: return "RESERVED_CLUSTER";
    case BAD_CLUSTER: return "BAD_CLUSTER";
    case LAST_CLUSTER: return "LAST_CLUSTER";
    case USED_CLUSTER: return "USED_CLUSTER";
    default: panic("bad value: %x\n", x);
    }
}

int fat32_fat_entry_type(uint32_t x) {
    // eliminate upper 4 bits.
    x = (x << 4) >> 4;
    switch(x) {
    case FREE_CLUSTER: 
    case RESERVED_CLUSTER:
    case BAD_CLUSTER:
        return x;
    }
    if(x >= 0x2 && x <= 0xFFFFFEF)
        return USED_CLUSTER;
    if(x >= 0xFFFFFF0 && x <= 0xFFFFFF6)
        panic("reserved value: %x\n", x);
    if(x >=  0xFFFFFF8  && x <= 0xFFFFFFF)
        return LAST_CLUSTER;
    panic("impossible type value: %x\n", x);
}

/****************************************************************************************
 * long file name helpers.
 */
// from wikipedia

int is_attr(uint32_t x, uint32_t flag) {
    return (x & flag) == flag;
}
uint8_t lfn_checksum(const uint8_t *pFCBName) {
   uint8_t sum = 0;

   for (int i = 11; i; i--)
      sum = ((sum & 1) << 7) + (sum >> 1) + *pFCBName++;
   return sum;
}

int lfn_is_last(uint8_t seqno) {
    return (seqno & (1<<6)) != 0;
}
int lfn_is_first(uint8_t seqno) {
    // remove is-last bit
    seqno &= ~(1<<6);
    return seqno == 1;
}
int lfn_is_deleted(uint8_t seqno) {
    return (seqno & 0xe5) == 0xe5;
}


void lfn_print_ent(lfn_dir_t *l, uint8_t cksum) {
    uint8_t n = l->seqno;
    printk("\tseqno = %x, first=%d, last=%d, deleted=%d\n", 
            n, lfn_is_first(n), lfn_is_last(n), lfn_is_deleted(n));
 
    uint8_t buf[27];
    memcpy(&buf[0],  l->name1_5,  10);
    memcpy(&buf[10], l->name6_11, 12);
    memcpy(&buf[22], l->name12_13, 4);
    buf[26] = 0;
    
    for(int i = 0; i < 26; i += 2) {
        if(buf[i] == 0 && buf[i+1] == 0)
            break;
        printk("lfn[%d] = '%c' = %x\n", i, buf[i], buf[i]);
    }
    printk("\tcksum=%x (expected=%x)\n", l->cksum, cksum);
}

#include "unicode-utf8.h"

static inline int lfn_terminator(uint8_t *x) {
    if(x[0] == 0 && x[1] == 0)
        return 1;
    if(x[0] == 0xff && x[1] == 0xff)
        return 1;
    return 0;
}

static char *utf8_append(char *buf, uint8_t *unicode, unsigned nbytes) {
    assert(nbytes%2 == 0);
    for(int i = 0; i < nbytes; i+=2) {
        if(lfn_terminator(unicode+i))
            break;
        char *utf8 = to_utf8(unicode[i] | (unicode[i+1]<<8));
        strcat(buf, utf8);
        buf += strlen(utf8);
    }
    return buf;
}

char *lfn_get_name(lfn_dir_t *s, int cnt) {
    static char filename[512];
    filename[0] = 0;
    char *buf = filename;
    for(int i = cnt-1; i >= 0; i--) {
        lfn_dir_t *l = s+i;
        assert(l->attr == FAT32_LONG_FILE_NAME);
        buf = utf8_append(buf, l->name1_5, sizeof l->name1_5);
        buf = utf8_append(buf, l->name6_11, sizeof l->name6_11);
        buf = utf8_append(buf, l->name12_13, sizeof l->name12_13);
    }
    *buf = 0;
    return filename;
}

void lfn_print(lfn_dir_t *s, int cnt, uint8_t cksum, int print_ent_p) {
    assert(cnt >= 1);

    // weird if larger.
    demand(cnt > 0 && cnt < 3, weird for cs140e if larger!);
    for(int i = 0;  i < cnt; i++) {
        assert(!lfn_is_deleted((s+i)->seqno)); 
        if(print_ent_p) {
            printk("lfn[%d]=\n", i);
            lfn_print_ent(s+i, cksum);
        }
        assert(s[i].cksum == cksum);
    }
    assert(lfn_is_last(s->seqno));
    assert(lfn_is_first((s+cnt-1)->seqno));
}

// reconstruct file name, return pointer to the dir-entry.
fat32_dir_t *fat32_dir_filename(char *name, fat32_dir_t *d, fat32_dir_t *end) {
    assert(!fat32_dirent_free(d));

    uint32_t x = d->attr;

    if(x == FAT32_LONG_FILE_NAME) {
        int cnt = 1, n = end-d;
        for(; cnt < n; cnt++)
            if(d[cnt].attr != FAT32_LONG_FILE_NAME)
                break;
        assert(cnt < n);
        assert(is_attr(d[cnt].attr, FAT32_DIR) || is_attr(d[cnt].attr, FAT32_ARCHIVE));
        strcpy(name, lfn_get_name((void*)d, cnt));
        return d+cnt;
    } else {
        assert(is_attr(x, FAT32_DIR) 
            || is_attr(x, FAT32_ARCHIVE) 
            || is_attr(x, FAT32_VOLUME_LABEL));
        int i=0,j=0, lower_case_p = 0;

        // macos?
        if(d->reserved0 == 0x18)
            lower_case_p = 1;

        for(; i < 8; i++) {
            char c = d->filename[i];
            // skip spaces.
            if(c == ' ')
                continue;
            if(lower_case_p && c >= 'A' && c <= 'Z')
                c += 32;
            name[j++] = c;
        }
        // implied b/n bytes 7 and 8
        name[j++] = '.';
        for(; i < 11; i++) {
            char c = d->filename[i];
            if(lower_case_p && c >= 'A' && c <= 'Z')
                c += 32;
            name[j++] = c;
        }
        name[j] =0;
        return d;
    } 
}

/****************************************************************************************
 * directory helpers.
 */


const char * fat32_dir_attr_str(int attr) {
    if(attr == FAT32_LONG_FILE_NAME)
        return " LONG FILE NAME";

    static char buf[128];
    buf[0] = 0;
    if(is_attr(attr, FAT32_RO)) {
        strcat(buf, "R/O");
        attr &= ~FAT32_RO;
    }
    if(is_attr(attr, FAT32_HIDDEN)) {
        strcat(buf, " HIDDEN");
        attr &= ~FAT32_HIDDEN;
    }

    switch(attr) {
    case FAT32_SYSTEM_FILE:     strcat(buf, " SYSTEM FILE"); break;
    case FAT32_VOLUME_LABEL:    strcat(buf, " VOLUME LABEL"); break;
    case FAT32_DIR:             strcat(buf, " DIR"); break;
    case FAT32_ARCHIVE:         strcat(buf, " ARCHIVE"); break;
    default: panic("unhandled attr=%x\n", attr);
    }
    return buf;
}

void fat32_dir_check(fat32_dir_t *d) {
    check_off(fat32_dir_t, filename,           0,      11);
    check_off(fat32_dir_t, attr,               11,      1);
    check_off(fat32_dir_t, ctime_tenths,       13,      1);
    check_off(fat32_dir_t, ctime,              14,      2);
    check_off(fat32_dir_t, create_date,        16,      2);
    check_off(fat32_dir_t, access_date,        18,      2);
    check_off(fat32_dir_t, hi_start,           20,      2);
    check_off(fat32_dir_t, mod_time,           22,      2);
    check_off(fat32_dir_t, mod_date,           24,      2);
    check_off(fat32_dir_t, lo_start,           26,      2);
    check_off(fat32_dir_t, file_nbytes,        28,      4);
}

int fat32_dirent_free(fat32_dir_t *d) {
    uint8_t x = d->filename[0];

    if(d->attr == FAT32_LONG_FILE_NAME) {
        lfn_dir_t *l = (void*)d;
        return lfn_is_deleted(l->seqno);
    }

    return x == 0 || x == 0xe5;
}

int fat32_dir_print(const char *msg, fat32_dir_t *d, int left) {
    printk("%s: ", msg);
    if(fat32_dirent_free(d)) {
        printk("\tdirent is not allocated\n");
        return 1;
    }
    int cnt = 1;

    if(d->attr == FAT32_LONG_FILE_NAME) {
        for(; cnt < left; cnt++) {
            if(d[cnt].attr != FAT32_LONG_FILE_NAME)
                break;
        }
        assert(cnt < left);
        assert(is_attr(d[cnt].attr, FAT32_DIR) || is_attr(d[cnt].attr, FAT32_ARCHIVE));
        printk("\n");
        lfn_print((void*)d, cnt, lfn_checksum(d[cnt].filename),0);

        char *name = lfn_get_name((void*)d, cnt);
        printk("\treconstructed filename = <%s>", name);

        d = &d[cnt];
        cnt++;
    } else if(!is_attr(d->attr, FAT32_DIR)) {
        printk("need to handle attr %x (%s)\n", d->attr, fat32_dir_attr_str(d->attr));
        if(is_attr(d->attr, FAT32_ARCHIVE))
            return 1;
    }
    printk("\n");
    printk("\tfilename      = <%s>\n", d->filename);
    printk("\tattr         = %x ", d->attr);
    if(d->attr != FAT32_LONG_FILE_NAME) {
        if(d->attr&FAT32_RO)
            printk(" [Read-only]\n");
        if(d->attr&FAT32_HIDDEN)
            printk(" [HIDDEN]\n");
        if(d->attr&FAT32_SYSTEM_FILE)
            printk(" [SYSTEM FILE: don't move]\n");
        printk("\n");
    }

    printk("\tctime_tenths = %d\n", d->ctime_tenths);
    printk("\tctime         = %d\n", d->ctime);
    printk("\tcreate_date   = %d\n", d->create_date);
    printk("\taccess_date   = %d\n", d->access_date);
    printk("\thi_start      = %x\n", d->hi_start);
    printk("\tmod_time      = %d\n", d->mod_time);
    printk("\tmod_date      = %d\n", d->mod_date);
    printk("\tlo_start      = %d\n", d->lo_start);
    printk("\tfile_nbytes   = %d\n", d->file_nbytes);
    return cnt;
}

/****************************************************************************************
 * general purpose utilities.
 */

void print_bytes(const char *msg, uint8_t *p, int n) {
    printk("%s\n", msg);
    for(int i = 0; i < n; i++) {
        if(i % 16 == 0)
            printk("\n\t");
        printk("%x, ", p[i]);
    }
    printk("\n");
}
void print_words(const char *msg, uint32_t *p, int n) {
    printk("%s\n", msg);
    for(int i = 0; i < n; i++) {
        if(i % 16 == 0)
            printk("\n\t");
        printk("0x%x, ", p[i]);
    }
    printk("\n");
}


void dump_sector(const char *msg, uint32_t sec) {
    unimplemented();
#if 0
    printk("sec=%d, %s:\n");

    static char sector[512];
    if(sd_read((void*)sector, 512, sec) < 0)
        panic("could not read from sd card\n");
    print_words("", (void*)sector, 512/4);
#endif
}

