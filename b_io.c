/**************************************************************
 * Class:  CSC-415-0# Fall 2021
 * Names:
 * Student IDs:
 * GitHub Name:
 * Group Name:
 * Project: Basic File System
 *
 * File: b_io.c
 *
 * Description: Basic File System - Key File I/O Operations
 *
 **************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> // for malloc
#include <string.h> // for memcpy
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "b_io.h"
#include "mfs.h"

#define MAXFCBS 20
#define B_CHUNK_SIZE 512

typedef struct b_fcb
{
  /** TODO add al the information you need in the file control block **/
  struct fs_diriteminfo *info;
  char *buf;  // holds the open file buffer
  int index;  // holds the current position in the buffer
  int buflen; // holds how many valid bytes are in the buffer
} b_fcb;

b_fcb fcb_array[MAXFCBS];

int startup = 0; // Indicates that this has not been initialized

// Method to initialize our file system
void b_init()
{
  // init fcb_array to all free
  for (int i = 0; i < MAXFCBS; i++)
  {
    fcb_array[i].buf = NULL; // indicates a free fcb_array
  }

  startup = 1;
}

// Method to get a free FCB element
b_io_fd b_getFCB()
{
  for (int i = 0; i < MAXFCBS; i++)
  {
    if (fcb_array[i].buf == NULL)
    {
      return i; // Not thread safe (But do not worry about it for this assignment)
    }
  }
  return (-1); // all in use
}

// Interface to open a buffered file
// Modification of interface for this assignment, flags match the Linux flags for open
// O_RDONLY, O_WRONLY, or O_RDWR
b_io_fd b_open(char *filename, int flags)
{
  // KEEP AN EYE FOR ALLOCS
  b_io_fd fd;

  //*** TODO ***:  Modify to save or set any information needed
  //
  //
  fdDir *opened = opendir(filename);
  struct fs_diriteminfo *info = readdir(opened);
  if (info == NULL)
  {
    closedir(opened);
    perror("diriteminfo failed");
    return NULL;
  }

  if (startup == 0)
    b_init(); // Initialize our system

  int fd = b_get_FCB();
  if (fd == -1)
  {
    closedir(opened);
    perror("FCB array is full.");
    return -1;
  }
  fcb_array[fd].info = info;
  fcb_array[fd].index = 0;
  fcb_array[fd].buf = malloc(B_CHUNK_SIZE);
  if (fcb_array[fd].buf == NULL)
  {
    closedir(opened);
    close(fd);
    perror("fcb buffer malloc failed\n");
    return -1;
  }
  return (fd); // all set
}

// Interface to seek function
int b_seek(b_io_fd fd, off_t offset, int whence)
{
  if (startup == 0)
    b_init(); // Initialize our system

  // check that fd is between 0 and (MAXFCBS-1)
  if ((fd < 0) || (fd >= MAXFCBS))
  {
    return (-1); // invalid file descriptor
  }

  return (0); // Change this
}

// Interface to write function
int b_write(b_io_fd fd, char *buffer, int count)
{
  if (startup == 0)
    b_init(); // Initialize our system

  // check that fd is between 0 and (MAXFCBS-1)
  if ((fd < 0) || (fd >= MAXFCBS))
  {
    return (-1); // invalid file descriptor
  }

  return (0); // Change this
}

// Interface to read a buffer

// Filling the callers request is broken into three parts
// Part 1 is what can be filled from the current buffer, which may or may not be enough
// Part 2 is after using what was left in our buffer there is still 1 or more block
//        size chunks needed to fill the callers request.  This represents the number of
//        bytes in multiples of the blocksize.
// Part 3 is a value less than blocksize which is what remains to copy to the callers buffer
//        after fulfilling part 1 and part 2.  This would always be filled from a refill
//        of our buffer.
//  +-------------+------------------------------------------------+--------+
//  |             |                                                |        |
//  | filled from |  filled direct in multiples of the block size  | filled |
//  | existing    |                                                | from   |
//  | buffer      |                                                |refilled|
//  |             |                                                | buffer |
//  |             |                                                |        |
//  | Part1       |  Part 2                                        | Part3  |
//  +-------------+------------------------------------------------+--------+
int b_read(b_io_fd fd, char *buffer, int count)
{

  if (startup == 0)
    b_init(); // Initialize our system

  // check that fd is between 0 and (MAXFCBS-1)
  if ((fd < 0) || (fd >= MAXFCBS))
  {
    return (-1); // invalid file descriptor
  }

  if (count <= 0)
  {
    return 0;
  }

  b_fcb *p_fcb = &fcb_array[fd];
  int bytes_read = 0;
  int to_copy = 0;
  int file_remaining = p_fcb->info->d_reclen - p_fcb->index;
  /*
  TL;DR pages through the file
  treat the file as a series of chunks
  track the read pointer progress in the chunk
  copy data from the read pointer forward looking to copy count bytes
  if end of chunk is reached, get the next chunk
  loop copying and getting chunks and copying
  until end of file or count bytes requested are copied
  if end of file is reached return immediately
  */
  do
  {
    // thread safe given one file per thread
    if (p_fcb->index % B_CHUNK_SIZE == 0)
    {
      LBAread(p_fcb->buf, 1,
              p_fcb->index / B_CHUNK_SIZE);
    }
    int left_in_chunk = B_CHUNK_SIZE - p_fcb->index % B_CHUNK_SIZE;
    to_copy = count < (left_in_chunk) ? to_copy : left_in_chunk;

    memcpy(buffer + bytes_read,
           p_fcb->buf + p_fcb->index % B_CHUNK_SIZE,
           to_copy);

    p_fcb->index += to_copy;
    bytes_read += to_copy;
    count -= to_copy;
    file_remaining -= to_copy;
    while (file_remaining >= B_CHUNK_SIZE)
    {
      LBAread(buffer, 1,
              p_fcb->index / B_CHUNK_SIZE);
    }
  } while (count > 0 && file_remaining > 0);
  return bytes_read;
}

// Interface to Close the file
int b_close(b_io_fd fd)
{
}
