/**************************************************************
 * Class:  CSC-415-01 Fall 2021
 * Names: Jacob Lawrence
 * Student IDs: 922384785
 * GitHub Name: liftcodesleep
 * Group Name: Coffee on the Rocks
 * Project: Basic File System
 *
 * File: b_io.h
 *
 * Description: Interface of basic I/O functions
 *
 **************************************************************/

#ifndef _B_IO_H
#define _B_IO_H
#include <fcntl.h>

#define FS_READ 0
#define FS_WRITE 1
#define FS_RDWR 2
#define FS_EXEC 4
#define FREE(x)  \
  if (x != NULL) \
  {              \
    free(x);     \
    x = NULL;    \
  }

typedef int b_io_fd;

b_io_fd b_open(char *filename, int flags);
int b_read(b_io_fd fd, char *buf, int count);
int b_write(b_io_fd fd, char *buf, int count);
int b_seek(b_io_fd fd, off_t offset, int whence);
int b_close(b_io_fd fd);
int malloc_wrap(size_t size, void **ppv, char *str);
#endif
