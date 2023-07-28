/**************************************************************
 * Class:  CSC-415-01 Summer 2023
 * Names: Matthew Bush, Oscar Galvez, Hyok In Kwon, Jacob Lawrence
 * Student IDs: 921619696, 911813414, 922373878, 922384785
 * GitHub Name: liftcodesleep
 * Group Name: Coffee On The Rocks
 * Project: Basic File System
 *
 * File: fsInit.c
 *
 * Description: Main driver for file system assignment.
 *
 *
 **************************************************************/

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "fsLow.h"
#include "mfs.h"
#include "vcb.h"
#include "dirEntry.h"
#include "b_bitmap.h"
#include "file_system_unit_test.h"

#define BLOCK_SIZE 512
#define MAGIC_NUMBER 1920213058

#define TEST_RUN 1

int initFileSystem(uint64_t numberOfBlocks, uint64_t blockSize)
{
  printf("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);
  /* TODO: Add any code you need to initialize your file system. */

  // Determine if you need to format the volume or not
  // Malloc a block of memory as your VCB pointer - per spec sheet
  vcb *vcbPointer = malloc(blockSize);

  // Report failure of malloc if occured - Abort program
  if (vcbPointer == NULL)
  {
    printf("Malloc has failed for VCB.\n");
    return -1;
  }

  // LBAread block 0 - Returns 1
  uint64_t blocksRead = LBAread(vcbPointer, 1, 0);

  // Report failure if no block is returned - Abort program
  if (blocksRead == 0)
  {
    printf("LBAread returned no block.");
    return -1;
  }

  // If signatures match, file system already initalized - Don't initalize again
  // Determine where magicNumber will be if already initalized
  if (vcbPointer->unique_volume_ID == MAGIC_NUMBER && !TEST_RUN)
  {
    printf("Magic Number validated.\n");
    // Initalize current working directory to root - Cleanup function in exitFileSystem();
    set_initial_directory();
    vcbPointer->free_block_map = load_free_space(19531, BLOCK_SIZE);
    return 0;
  }

  // Initalize the values in your VCB - per specs
  initVCB(vcbPointer);

  // Initalize free space
  vcbPointer->free_block_map = init_free_space(19531, BLOCK_SIZE);
  // Initalize the root directory
  vcbPointer->root_location = init_dir(10, NULL);

  // Set the values returned from above in the VCB
  // LBAwrite the VCB to block 0 - Hexdump will validate
  LBAwrite(vcbPointer, 1, 0);

  // Initalize current working directory to root - Cleanup function in exitFileSystem();
  set_initial_directory();

  if (TEST_RUN)
  {
    file_system_unit_tests();
  }

  // // Initalize current working directory to root - Cleanup function in exitFileSystem();
  set_initial_directory();

  return 0;
}

void exitFileSystem()
{
  printf("System exiting\n");

  // Cleanup
  free_directory();
}