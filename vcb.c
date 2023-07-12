#include <stdio.h>

#include "vcb.h"

#define BLOCK_SIZE 512

void initVCB(vcb *vcb) {
    // Initalize the values in your VCB
    vcb->unique_volume_ID = 0; // Determine how to generate 
	vcb->total_blocks = 0;
	vcb->free_block_map = 0; // Initalize free space - Bitmap - Replace
	vcb->block_size = BLOCK_SIZE;
	vcb->max_file_name_length = 0;
	vcb->root_location = 0;
    //direct* res_root;

	vcb->max_file_path_length = 0;
	vcb->file_count = 0;
	vcb->creation_time = 0;
	vcb->last_mounted_time = 0;
}