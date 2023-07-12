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
#include "dirEntry.h"


int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize)
	{
	printf ("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);
	/* TODO: Add any code you need to initialize your file system. */

	// Determine if you need to format the volume or not

	// Malloc a block of memory as your VCB pointer - per spec sheet
	vcb * test = malloc(blockSize);

	// LBAread block 0
	//test = LBAread(test, 1, 0); LBAread block zero?

	// If signatures match, vcb already initalized - Don't initalize again
	// Determine where magicNumber will be if already initalized
	// if (magicNumber == test->unique_volume_ID)
	// {
	// 	return;
	// }

	// Initalize the values in your VCB - Contained within function
	initVCB(test);

	//Initalize free space
	//Initalize the root directory
	//vcb->root_location = init_dir()?
	//Set the values returned from above in the VCB
	//LBAwrite the VCB to block 0;

	return 1;
	}
	
	
void exitFileSystem ()
	{
	printf ("System exiting\n");
	}