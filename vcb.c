#include <stdio.h>

#include "vcb.h"
#include "dirent.h"

#define BLOCK_SIZE 512

// Initalize the values in your VCB
void initVCB(vcb *vcb) {

    vcb->unique_volume_ID = 1920213058; // Determine how to generate - Hardcoded
	vcb->total_blocks = 1;
	vcb->free_block_map = 1; // VCB occupies block 0 - Initalize to 1
	vcb->block_size = BLOCK_SIZE;
	vcb->max_file_name_length = 0;
	vcb->root_location = 0;
    //dirent* res_root;
	//vcb->res_root = 0; // Figure out what to assign here

	vcb->max_file_path_length = 0;
}