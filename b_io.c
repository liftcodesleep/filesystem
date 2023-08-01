/**************************************************************
 * Class:  CSC-415-01 Fall 2021
 * Names: Jacob Lawrence
 * Student IDs: 922384785
 * GitHub Name: liftcodesleep
 * Group Name: Coffee on the Rocks
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
// if we read then write in the middle of the block
// then what, commenter above, what then?
typedef struct b_fcb
{
  /** TODO add all the information you need in the file control block **/
  struct fs_diriteminfo *info;
  int buffer_index; // holds the current position of the buffer in the file
  int file_index;   // holds the current position in the file
  int buflen;       // holds how many valid bytes are in the buffer
  short perm;
  char *buf; // holds the open file buffer
} b_fcb;

int malloc_wrap(size_t size, void **ppv, char *str)
{
  *ppv = malloc(size);
  if (*ppv == NULL)
  {
    printf("%s malloc failed", str);
  }
  return *ppv == NULL;
}
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
  dir_and_index *di = parse_path(filename);

  if (di == NULL)
  {
    printf("parse_path failed");
    return -1;
  }
  if (startup == 0)
  {
    b_init(); // Initialize our system
  }
  //*** TODO ***:  Modify to save or set any information needed
  //
  //
  if (flags & (O_WRONLY & O_RDWR))
  {
    printf("Illegal combination of flags\n");
    return -1;
  }
  if (flags & O_CREAT)
  {
    if (di->index != -1)
    {
      printf("Create invalid, file already exists.\n");
      return -1;
    }
    if (fs_mkfil(filename, 0777))
    {
      printf("mkfile failed\n");
      return -1;
    }
    di = parse_path(filename);
  }
  fd = b_getFCB();
  if (flags & O_WRONLY)
  {
    fcb_array[fd].perm |= FS_WRITE;
  }
  else if (flags & O_RDWR)
  {
    fcb_array[fd].perm |= FS_RDWR;
  }
  else
  {
    fcb_array[fd].perm = FS_READ;
  }
  if (flags & O_TRUNC)
  {
    printf("truncating\n");
    if (di->index == -1)
    {
      b_close(fd);
      printf("Cannot truncate nonexistent file\n");
      return -1;
    }
    di->dir[di->index].size = 0;
    extent_remove_blocks(di->dir[di->index].extents, 0, 0);
    di->dir[di->index].time_last_modified = (unsigned long)time(NULL);
    LBAwrite(di->dir, di->dir->extents->count, di->dir->extents->start);
  }

  fdDir *opened = fs_opendir(filename);

  struct fs_diriteminfo *info;
  // printf("Name of path: %s\n", filename);
  // printf("Index of opening: %d\n", di->index);
  // printf("TEST: %s\n", di->dir[2].name);

  for (int i = 0; i <= di->index; i++)
  {
    info = fs_readdir(opened);
  }

  // printf("b_open: info.name: %s\n", info->d_name);
  // printf("b_open: info.len: %d\n", info->d_reclen);
  // printf("b_open: info.len: %p\n", &info->location);
  // printf("?\n");
  // printf("Name of file reading: %s\n", info->d_name);
  if (info == NULL)
  {
    b_close(fd);
    fs_closedir(opened);
    printf("diriteminfo failed\n");
    return -1;
  }
  if (fd == -1)
  {
    b_close(fd);
    fs_closedir(opened);
    perror("FCB array is full.");
    return -1;
  }
  fcb_array[fd].info = info;
  fcb_array[fd].file_index = 0;
  fcb_array[fd].buffer_index = -1; // buffer is not in file/truly initialized
  if (malloc_wrap(B_CHUNK_SIZE, (void **)&fcb_array[fd].buf, "fcb_array[fd].buf"))
  {
    close(fd);
    fs_closedir(opened);
    return -1;
  }

  return (fd);
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

  if ((whence < 0) || (whence > 2))
  {
    printf("Invalid 'whence' directive.\n");
    return (-1);
  }

  // Fail condition - Prevention from potentially accesssing uninitalized variables
  if (fcb_array[fd].file_index == -1) {
    printf("b_seek failed.\n");
    return (-1);
  }


  // No ways to detect multiple options other than those that exceed the value of 3
  // The file offset is set to offset bytes
  if (whence == SEEK_SET)
  {
    fcb_array[fd].file_index = offset;
  }

  // The file offset is set to its current location plus offset bytes
  if (whence == SEEK_CUR)
  {
    fcb_array[fd].file_index = fcb_array[fd].file_index + offset;
  }

  // The file offset is set to the size of the file plus offset bytes
  if (whence == SEEK_END)
  {
    fcb_array[fd].file_index = fcb_array[fd].info->d_reclen + offset;
  }

  // Returns the resulting offset location as measured in bytes from the
  // beginning of the file
  return (fcb_array[fd].file_index);
}

// Interface to write function
// write needs to check if/when to update buffer
// if write writes a block in the fcb buffer, the buffer needs an update
int b_write(b_io_fd fd, char *buffer, int count)
{

  if (startup == 0)
    b_init(); // Initialize our system
  // check that fd is between 0 and (MAXFCBS-1)
  if ((fd < 0) || (fd >= MAXFCBS))
  {
    return (-1); // invalid file descriptor
  }
  if (fcb_array[fd].perm == 0)
  {
    printf("No write access fuckouttahere\n");
    return -1;
  }

  int wrote_bytes = 0;
  dir_and_index *di = parse_path(fcb_array[fd].info->d_name);
  pextent loc = di->dir[di->index].extents;

  // printf("Reading extent for %s\n", di->dir[di->index].name);

  // Alloc blocks if needed
  if ((extent_size(loc) - 1) < (count - 1) / B_CHUNK_SIZE + 1)
  {
    pextent new_blocks = allocate_blocks((count - 1) / B_CHUNK_SIZE + 1 - (extent_size(loc) - 1), 1);

    int i = 0;
    pextent temploc = extent_at_index(new_blocks, i);

    while (temploc != NULL)
    {
      extent_append(loc, temploc->start, temploc->count);
      temploc = extent_at_index(new_blocks, ++i);
    }
    LBAwrite(di->dir, di->dir->extents->count, di->dir->extents->start);
  }

  printf("In extent %d %d\n", loc[1].start, loc[1].count);

  // write if not starting from the begining of a block
  if (fcb_array[fd].file_index % B_CHUNK_SIZE != 0)
  {
    LBAread(fcb_array[fd].buf, 1, extent_block_to_LBA(loc + 1, fcb_array[fd].file_index / B_CHUNK_SIZE));
    memcpy(fcb_array[fd].buf + fcb_array[fd].file_index % B_CHUNK_SIZE, buffer, fcb_array[fd].file_index % B_CHUNK_SIZE);
    LBAwrite(fcb_array[fd].buf, 1, extent_block_to_LBA(loc + 1, fcb_array[fd].file_index / B_CHUNK_SIZE));
    wrote_bytes += fcb_array[fd].file_index % B_CHUNK_SIZE;
    fcb_array[fd].file_index += fcb_array[fd].file_index % B_CHUNK_SIZE;
  }
  if (wrote_bytes == count)
  {
    return count;
  }

  // write in all full blocks
  while (wrote_bytes + B_CHUNK_SIZE < count)
  {

    LBAwrite(buffer + wrote_bytes, 1, extent_block_to_LBA(loc + 1, fcb_array[fd].file_index / B_CHUNK_SIZE));
    wrote_bytes += B_CHUNK_SIZE;
    fcb_array[fd].file_index += B_CHUNK_SIZE;
  }
  if (wrote_bytes == count)
  {
    return count;
  }

  // write if not ending at a block
  LBAread(fcb_array[fd].buf, 1, extent_block_to_LBA(loc + 1, fcb_array[fd].file_index / B_CHUNK_SIZE));
  // printf("TRYING TO WRITE %d\n", extent_block_to_LBA(loc + 1, fcb_array[fd].file_index / B_CHUNK_SIZE));
  memcpy(fcb_array[fd].buf, buffer + wrote_bytes, count - wrote_bytes);
  LBAwrite(fcb_array[fd].buf, 1, extent_block_to_LBA(loc + 1, fcb_array[fd].file_index / B_CHUNK_SIZE));
  wrote_bytes += count - wrote_bytes;
  return wrote_bytes;
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
  if (fcb_array[fd].perm == 1)
  {
    printf("No read access to file permitted\n");
    return -1;
  }
  b_fcb *p_fcb = &fcb_array[fd];
  int bytes_read = 0;
  int to_copy = 0;
  int file_remaining = p_fcb->info->d_reclen - p_fcb->file_index;
  int buffer_remaining = p_fcb->buflen - p_fcb->buffer_index;
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
  printf("b_read: fcb_array[fd].info.d_name: %s\n", fcb_array[fd].info->d_name);
  do
  {
    // thread safe given one file per thread
    if (p_fcb->file_index % B_CHUNK_SIZE == 0)
    {
      // TODO make sure this actually works
      // intent is read directly to their buffer
      // if (count % B_CHUNK_SIZE == 0)
      // {
      //   LBAread(buffer, count / B_CHUNK_SIZE, p_fcb->file_index);
      //   // TODO Update counts and stuff
      //   continue;
      // }
      LBAread(p_fcb->buf, 1, extent_block_to_LBA(fcb_array[fd].info->location, p_fcb->file_index / B_CHUNK_SIZE));
    }
    int left_in_chunk = B_CHUNK_SIZE - p_fcb->file_index % B_CHUNK_SIZE;
    to_copy = count < (left_in_chunk) ? to_copy : left_in_chunk;
    // TODO buflen incorrect
    memcpy(buffer + p_fcb->buflen,
           p_fcb->buf + p_fcb->file_index % B_CHUNK_SIZE,
           to_copy);
    p_fcb->file_index += to_copy;
    p_fcb->buflen += to_copy;
    count -= to_copy;
    file_remaining -= to_copy;
  } while (count > 0 && file_remaining > 0);
  return p_fcb->buflen;
}
// Interface to Close the file
int b_close(b_io_fd fd)
{
  fcb_array[fd].info = NULL; // possible memory leak
  FREE(fcb_array[fd].buf);
  return 0;
}
