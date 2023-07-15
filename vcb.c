#include <stdio.h>

#include "vcb.h"
#include "dirent.h"

#define BLOCK_SIZE 512

// Initalize the values in your VCB
void initVCB(vcb *vcb)
{
  vcb->unique_volume_ID = 1920213058;
  vcb->total_blocks = 19531;
  vcb->free_block_map = 1; // Initalize to 1 - Will be reassigned
  vcb->block_size = BLOCK_SIZE;
  vcb->max_file_name_length = 100;
  vcb->root_location = 7;

  vcb->max_file_path_length = 100;
}