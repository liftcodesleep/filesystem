/**************************************************************
* Class:  CSC-415-0# Fall 2021
* Names: 
* Student IDs:
* GitHub Name:
* Group Name:
* Project: Basic File System
*
* File: fsInit.c
*
* Description: Main driver for file system assignment.
*
* This file is where you will start and initialize your system
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

#define BLOCK_SIZE 512
#define MAGIC_NUMBER 1920213058

int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize)
	{
	printf ("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);
	/* TODO: Add any code you need to initialize your file system. */

	// Determine if you need to format the volume or not
	// Malloc a block of memory as your VCB pointer - per spec sheet
	vcb * test = malloc(blockSize);

	// Report failure of malloc if occured - Abort program
	if (test == NULL)
	{
		printf("Malloc has failed for VCB.\n");
		return -1;
	}

	// LBAread block 0 - Returns 1
	uint64_t blocksRead = LBAread(test, 1, 0);

	// Report failure if no block is returned - Abort program
	if (blocksRead == 0)
	{
		printf("LBAread returned no block.");
		return -1;
	}

	// If signatures match, vcb already initalized - Don't initalize again
	// Determine where magicNumber will be if already initalized
	// Using hard coded number to test - Successful
	if (test->unique_volume_ID == MAGIC_NUMBER)
	{
		return 0;
	}

	// Initalize the values in your VCB - per specs
	initVCB(test);

	//Initalize free space

	//Initalize the root directory
	//init_dir(50, NULL); Return an int to assign to root_location?
	//vcb->root_location = init_dir()?

	//Set the values returned from above in the VCB
	//LBAwrite the VCB to block 0 - Hexdump will validate
	LBAwrite(test, 1, 0);

	return 0;
	}
	
	
void exitFileSystem ()
	{
	printf ("System exiting\n");
	}