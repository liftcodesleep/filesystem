/**************************************************************
* Class: CSC-415-01 Summer 2023
* Name: Oscar Galvez
* Student ID: 911813414
* Group Name: Coffee on the Rocks
* Project: File System Project
*
* File: vcb.c
*
* Description: The main purpose of this file is to initalize the
*              values that comprise of the Volume Control Block.
*              initVCB is called if it is determined that the
               volume must be formatted due to a mismatch of the
               unique_volume_ID.
*
**************************************************************/

#include <stdio.h>

#include "vcb.h"
#include "dirEntry.h"

#define BLOCK_SIZE 512

vcb * functionVCB;

// Initalize the values in your VCB
void initVCB(vcb *vcb)
{
  vcb -> unique_volume_ID = 1920213058;
  vcb -> total_blocks = 19531;
  vcb -> bytes_in_use = 3584; // Byte offset considering 7 blocks in use
  vcb -> direntry_size = sizeof(direntry); // Byte size of direntry struct
  vcb -> free_block_map = 1; // Initalize to 1 - Will be reassigned

  // Assuming each init_dir uses 4 blocks every time, 1 would signifed 4 blocks used
  // from the root location. Can use this variable to iterate and search through
  // all dirents in use if they are allocated and specified here.
  vcb -> direntry_in_use = 1;
  vcb -> block_size = BLOCK_SIZE;
  vcb -> max_file_name_length = 100;
  vcb -> root_location = 7;
  vcb -> max_file_path_length = 256;
}

int calculateOffset() {
  
  // If Block 7 being used, and 128 bytes currently in use while assigning another direntry
  // 3752 - (7 * 512) -> 3752 - 3584 = 168
  int offset = (functionVCB -> bytes_in_use - (functionVCB -> blocks_in_use * BLOCK_SIZE));
}

unsigned long blockSize() {
  return functionVCB -> block_size;
}

// Call to increment amounts of dirents in use
void incrementDirent() {
  functionVCB -> direntry_in_use = functionVCB -> direntry_in_use + 1;
}