

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> // for malloc
#include <string.h> // for memcpy
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "b_io.h"
#include "mfs.h"

#include "dirEntry.h"


#define MAXFCBS 20
#define B_CHUNK_SIZE 512

// if we read then write in the middle of the block

typedef struct b_fcb
{
  /** TODO add al the information you need in the file control block **/
  struct fs_diriteminfo *info;
  int buffer_index; // holds the current position in the buffer
  int file_index;   // holds the current position in the file
  int buflen;       // holds how many valid bytes are in the buffer
  char *buf;        // holds the open file buffer
  char perm;        // 0 = O_RDONLY 1 = O_WRONLY 2 = O_RDWR
} b_fcb;


b_fcb fcb_array[MAXFCBS];


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


  pextent file_extent = fcb_array[fd].info->location;

  int file_block_size = extent_size(file_extent);


  int blocks_needed = (count+fcb_array[fd].info->d_reclen) / B_CHUNK_SIZE + 1;

  printf("Amount of blocks needed: %d\n", blocks_needed);

  pextent current_new_extent = NULL;
  if(blocks_needed > file_block_size)
  {
  	pextent new_extent = allocate_blocks( blocks_needed - file_block_size, 1 );

  	current_new_extent = extent_at_index(new_extent, 0);
  	int i = 0;
  	while(current_new_extent !=  NULL)
  	{
  		extent_append( file_extent, current_new_extent->start, current_new_extent->count);
  		current_new_extent = extent_at_index(new_extent, 0);
  	}

  }
  int wrote_bytes = 0;
  current_new_extent = extent_at_index(file_extent, 0);
  int i = 0;
  while(current_new_extent !=  NULL)
  {


  	//extent_append( file_extent, current_new_extent->start, current_new_extent->count);
  	LBAwrite(buffer+((i+current_new_extent->count)*B_CHUNK_SIZE), current_new_extent->count, current_new_extent->start );
  	wrote_bytes += urrent_new_extent->count*B_CHUNK_SIZE;
  	current_new_extent = extent_at_index(file_extent, 0);
  }


  return wrote_bytes; // Change this
}
