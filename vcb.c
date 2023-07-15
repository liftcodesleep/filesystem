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