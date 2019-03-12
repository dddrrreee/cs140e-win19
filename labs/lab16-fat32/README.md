## Build a simple FAT32 read-only file system.  Use it to fork/exec a process.

This is the last full lab of the quarter.  We will do a classic hello world: 
  1. Write a simple read-only file system that can read from your SD card.
  2. Use it to read in a `hello.bin`.
  3. Use your system call lab to run this program and echo the output.


### Background: File systems over-simplified.

Our r/pi's (and most SD card devices) expect to work with Microsoft's
FAT32 file system.  On the plus side, FAT32 is relatively simple and,
because it's so widely-used, fairly-well documented.  On the negative end,
it has a really ugly way to handling filenames longer than 8 characters.
(You can look at the LFN code in the `fat32-helper.c` to get a feel
for this).

The big picture:
   1. From our view, disk is an array.  Each entry is a 512-byte sector.
   The first sector (again from our view, if not from the device's)
   is named 0, the second, 1, etc.  To amortize overhead, devices let
   you read multiple contiguous sectors with a single operation.

   2. Over-simplifying: file systems are trees of files and directories.
   Directories have pointers to files and directories.  Files have
   pointers to data blocks.  For performance and space reasons, most
   file systems do not represent files as contiguous arrays of pointers
   to data (as we would with a simple one-level page table), 
   but instead use an irregular hierarchical structure of
   pointers to different types of structures that eventually point to
   data blocks.  For example, the Unix FFS file system uses an inode to
   hold the initial 12 or so pointers, an indirect block to hold the next
   1024 or so, double indirect blocks, etc.  You've seen these tradeoffs
   and methods in page tables --- the different here is that most files
   are small, and almost all files are accessed sequentially from the
   beginning to the end, so the data structures to navigate them are
   tuned differently.

   3. If we had to only store these trees in in-memory, building a file
      system would be a straightforward assignment in an intro programming
      class.  However, because we have to store the file system on disk,
      file systems were the single largest systems research topic in the
      80s and 90s.  (Mostly b/c each access is/was millions of cycles,
      and you cannot write everything you need atomically.)

      For our lab today, the main issue with disk storage is that 
      pointers will be more awkward since we have to use disk addresses
      instead of memory addresses.  As a result `*p` becomes a bunch
      of function calls and table calculations (similar, for real
      reasons, to what is going on behind the scenes for virtual memory).
      A related issue, is finding the tables --- in our case, the FAT ---
      and understanding what its domain and range is.

### Overview: FAT32.

Our SD cards are broken down as follows:

  - Sector 0 on the SD card: the master boot record (`mbr`, 512 bytes).
    Holds four partition tables giving the disk offset (in sectors)
    where the partition starts and the number of sectors in it.
    For us, the first partition should be as big as the SD card, and
    the rest should be empty.  The partition type should be FAT32.
    (Print and check.)

  - Sector 0 in the partion: the FAT32 boot sector (512 bytes).  FAT32 breaks the
    disk into many fields, some useful ones:
       - Number of sectors in each cluster (`sec_per_cluster`).
       - Total file system size (`nsec_in_fs`),
       - How many sectors the FAT table is stored in (`nsec_per_fat`).
       - `reserved_area_nsec` the number of blocks to skip before reading the 
       FAT table (which is the sector right after the reserved area).

    You can check the boot sector is valid (`fat32_volume_id_check`)
    and print it (`fat32_volume_id_print`).

   - The FAT table itself: read in `nsec_per_fat` sectors starting right after the
     `reserved_area_nsec`.  The FAT table is often replicated back-to-back, so you
     can read in both and check they are identical.

   - The root directory: located by reading `sec_per_cluster` worth of
     data from cluster 2.

Further reading --- the first three are good summaries, the latter have more detail:

  1. [aeb's summary](https://www.win.tue.nl/~aeb/linux/fs/fat/fat-1.html).
  2. [forensic wiki](https://www.forensicswiki.org/wiki/FAT).
  3. [data structure layout](http://www.c-jump.com/CIS24/Slides/FileSysDataStructs/FileSysDataStructs.html).
  4.  [wikipedia](https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system).
  A full description.  Perhaps more detail than you need, but useful
  for all the weird flags that can pop up.
  5. [linux source](https://elixir.bootlin.com/linux/latest/source/fs/fat/dir.c).

#### Checkoff


  1. A working sdcard driver that you steal from somewhere else and adapt.
  2. Read and echo the contents of `config.txt` on your SD card.
  3. Read in an jumped to a `hello.bin` program (used the ones from the shell lab).

Extensions:
  1. Run the `hello` program in a virtual address range using the last lab.
  2. Make multiple drivers work and cross check they do the same reads
  and writes when you run them (as we did in lab3).

### Part 1: An SD card driver.

In general, we prefer to write our own code from primary sources
(datasheets, ARM manuals):
  1. Otherwise you really don't know what you're talking about and (related);
  2. Whenever we do so, we realize how broken most of the other code out there is.

However, for larger drivers, we'll lower our standards and steal other
people's code.  One aspect of doing so is being comfortable ripping code
out of its original source-tree and making shims to have it work in yours.
Today's lab will be good practice for doing so.

   1. Go to [bzt's repo](https://github.com/bztsrc/raspi3-tutorial/tree/master/15_writesector)
   and get `sd.c` and `sd.h`.

   2. Make these compile on our code.  

   3. You'll have to `#define` various functions (`printf` to `printk`),
   but there shouldn't be much difference.    In general, we prefer
   external changes versus changing the code directly so that we minimize
   the work if we want to pull updates from the original code base.

   4. This code is for the 64-bit rpi3 so you'll get a few warnings about
   printing code; delete the problem areas.  You'll also get undefined
   reference to a division function, which you can get rid of since the
   division is by a constant.

Implement the helper routine:

   - `sec_read(uint32_t lba, uint32_t nsec)` which will allocate a buffer
   needed to hold `nsec` worth of data, read the data in, and return a pointer to
   the buffer.

After 10-20 minutes you should have a working driver.  

### Part 2: Define and read-in the master boot record.

As stated earlier, the master boot record is at offset 0 of the SD card.  Define
the master boot record using the layout from:
   - [slides](http://www.c-jump.com/CIS24/Slides/FileSysDataStructs/FileSysDataStructs.html)

There are some pictures here:
   - [some pictures](https://www.pjrc.com/tech/8051/ide/fat32.html).

Reading in one sector should give it to you.  Check that:
   - It's last 16 bits equal `0xaa55`.
   - It has a single non-empty parition with type equal to either `0xb` or `0xc`.

### Part 3: read in and setup the FAT.

Here you will use the boot record to setup the `fat32_fs_t` structure.

   1. Use the MBR to read in the FAT32 boot sector.  Its the first sector in the 
   partition; there is a structure definition for it in `fat32_boot_sec_t`.

   2. Check it with `fat32_volume_id_check` and print it to make sure the values make sense.

   3. Use its fields to implement the `fat32_mk` routine,
   which will define the important pieces for your FAT32 FS.
   A reasonable description of the information you need is
   [here](https://www.pjrc.com/tech/8051/ide/fat32.html)

### Part 4: read in the root directory.

You'll get the root directory, read it in, and print it.

   1. Use your fat structure to compute where cluster 2 is (the root directory).
   2. Read it in.
   3. Implement the `fat32_to_dirent` code to walk down the list of directories and
   use our helper functions to translate them to something more reasonable.

### Part 5: read in and print `config.txt`

If everything has gone according to plan:
   1. You should be able to get the cluster id from your directory structure.
   2. Then implement `fat32_read_file` to walk down the fat, reading it into
   an allocated buffer.   
   3. When the entry in the fat is `LAST_CLUSTER` you know you've reached the end.

### Part 6: read in and run `hello.bin`

   1. Store a `hello.bin` linked to a different address range onto your your SD card
      (use the code from `lab10-shell/hello-fixed/`).
   2. Read it into a buffer at the required range.
   3. Jump to it.

For special points: 
   1. Use your lab from last week to map the bytes for a regular `hello.bin` 
   into its own virtual address space.
   2. Switch to the address space and run it.
