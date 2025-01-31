/**************************************************************
 * Class:  CSC-415
 * Name: Professor Bierman
 * Student ID: N/A
 * Project: Basic File System
 *
 * File: mfs.h
 *
 * Description:
 *	This is the file system interface.
 *	This is the interface needed by the driver to interact with
 *	your filesystem.
 *
 **************************************************************/
#ifndef _MFS_H
#define _MFS_H
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include "fsLow.h"
#include "b_io.h"
#include "dirEntry.h"
#include "extent.h"

#include <dirent.h>
#define FT_REGFILE DT_REG
#define FT_DIRECTORY DT_DIR
#define FT_LINK DT_LNK

#ifndef uint64_t
typedef u_int64_t uint64_t;
#endif
#ifndef uint32_t
typedef u_int32_t uint32_t;
#endif

// This structure is returned by fs_readdir to provide the caller with information
// about each file as it iterates through a directory
struct fs_diriteminfo
{
  unsigned short d_reclen; /* length of this record */
  unsigned char fileType;
  char d_name[256]; /* filename max filename is 255 characters */
  extent *location;
};

// This is a private structure used only by fs_opendir, fs_readdir, and fs_closedir
// Think of this like a file descriptor but for a directory - one can only read
// from a directory.  This structure helps you (the file system) keep track of
// which directory entry you are currently processing so that everytime the caller
// calls the function readdir, you give the next entry in the directory
typedef struct
{
  /*****TO DO:  Fill in this structure with what your open/read directory needs  *****/
  unsigned short d_reclen;         /* length of this record */
  unsigned short dirEntryPosition; /* which directory entry position, like file pos */
  // DE *	directory;			/* Pointer to the loaded directory you want to iterate */
  struct fs_diriteminfo *di; /* Pointer to the structure you return from read */
} fdDir;

// Key directory functions

typedef struct dir_and_index
{
  direntry *dir;
  int index;

} dir_and_index;

dir_and_index *parse_path(const char *pathname);

// JOHNNY***********
int fs_mkdir(const char *pathname, mode_t mode);
int fs_mkfil(const char *pathname, mode_t mode);
int fs_rmdir(const char *pathname);
//*****************

// Directory iteration functions

// JOHNNY***********
fdDir *fs_opendir(const char *pathname);
//*****************

// MATT*************
struct fs_diriteminfo *fs_readdir(fdDir *dirp);
int fs_closedir(fdDir *dirp);
//*****************

// Misc directory functions

// OSCAR****************
char *fs_getcwd(char *pathname, size_t size);
int fs_setcwd(char *pathname); // linux chdir
//*********************

// JACOB********************
int fs_is_file(char *filename); // return 1 if file, 0 otherwise
int fs_is_dir(char *pathname);  // return 1 if directory, 0 otherwise
//*************************
int fs_delete(char *filename); // removes a file

// NEW
int allocateBuffer();        // Allocate memory to load 4 blocks to traverse - For testing
int set_initial_directory(); // Set current working directory to root
void free_directory();       // Free current_working_directory memory

// This is the strucutre that is filled in from a call to fs_stat
struct fs_stat
{
  off_t st_size;        /* total size, in bytes */
  blksize_t st_blksize; /* blocksize for file system I/O */
  blkcnt_t st_blocks;   /* number of 512B blocks allocated */
  time_t st_accesstime; /* time of last access */
  time_t st_modtime;    /* time of last modification */
  time_t st_createtime; /* time of last status change */

  /* add additional attributes here for your file system */
};

int fs_stat(const char *path, struct fs_stat *buf);

#endif
