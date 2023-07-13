#include <stdio.h>
#include "dirEntry.h"

typedef struct vcb {
    long unique_volume_ID; // unique ID / magic number
    unsigned long total_blocks;
    unsigned int free_block_map; // block number of bitmap startin block - Bierman
    unsigned int block_size; // 512
    unsigned int max_file_name_length;
    unsigned int root_location;
    //dirent* res_root; // This is not valid on disk must be init every time

    unsigned int max_file_path_length; // File System
    unsigned int file_count; // Is this covered in free blocks?
    unsigned long creation_time; // Help for debugging
    unsigned long last_mounted_time; // Help for debugging
} vcb;

// Initalize Volume Control Block - Populate variables
void initVCB(vcb *vcb);