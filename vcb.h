/**************************************************************
* Class: CSC-415-01 Summer 2023
* Name: Oscar Galvez
* Student ID: 911813414
* Group Name: Coffee on the Rocks
* Project: File System Project
*
* File: vcb.h
*
* Description:  Definition of vcb type and the prototypes
*               of the functions accessible by user applications
*
**************************************************************/


#ifndef VCB
#define VCB


#include <stdio.h>
// #include "dirEntry.h"

typedef struct vcb
{
  long unique_volume_ID; // unique ID / magic number
  unsigned long total_blocks;
  unsigned long bytes_in_use; // Track number of bytes currently in use - Calculate offset
  unsigned long blocks_in_use; // Track number of blocks in use - Calculate offset
  unsigned int direntry_size; // Bytes direntry struct occupies - 160 bytes
  unsigned int free_block_map; // block number of bitmap startin block - Bierman
  unsigned int block_size;     // 512
  unsigned int dirents_in_use; // Track number of dirent blocks in use
  unsigned int max_file_name_length;
  unsigned int root_location;
  // dirent* res_root; // This is not valid on disk must be init every time

  unsigned int max_file_path_length; // File System
} vcb;

// Initalize Volume Control Block - Populate variables
void initVCB(vcb *vcb);
int calculateOffset();
unsigned long blockSize();
void incrementDirent();

#endif