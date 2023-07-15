/**************************************************************
 * Class:  CSC-415-01 Summer 2023
 * Names: Matthew Bush
 * Student IDs: 921619696
 * GitHub Name: fattymatty15
 * Group Name: Coffee on the Rocks
 * Project: Basic File System
 *
 * File: fsInit.c
 *
 * Description: This extents file is used to manage blocks of data within the file system. 
 * The functions include printing the location and count of extents, retrieving 
 * the block number based on an index, appending new extents while maintaining 
 * contiguity, a placeholder function for removing blocks, initializing the extent 
 * array, calculating the total number of blocks, and freeing the extent memory. 
 *
 **************************************************************/
#ifndef EXTENT_H
#define EXTENT_H

#include <stdio.h>
#include <stdlib.h>
#include "fsLow.h"
#include "b_bitmap.h"
#define EXTENT_SIZE 3

#define SENTINAL_VALUE 0xffffffff

typedef unsigned int uint;

////// Change extent to an array of struct

/*
 * Initializes the extent to its initial state.
 */
void extent_init(extent *extent);

void extent_free(extent *extent);

/*
 * Returns:
 *   - The block number at the specified index.
 * Error:
 * Reutrns -1
 */
int extent_get_block_num(extent *extent, uint index);

/*
 * Adds more blocks to the extent. Also merges the extent
 * Returns:
 *   - The number of blocks successfully added.
 */
unsigned int extent_append(extent *extent, uint block_number, uint count);

/*
 * Removes blocks from the extent.
 * Returns:
 *   - The number of blocks successfully removed.
 */
unsigned int extent_remove_blocks(extent *extent, uint block_number, uint count);

/*
 * Returns:
 *   - The total number of blocks in the extent.
 */
unsigned int extent_get_total_blocks(extent *extent);

/*
 * merges the second extent into the first extent
 * Returns:
 *   - The amount of blocks merged
 *
 */
int extent_merge(extent *first_extent, extent *second_extent);

/*
 * For debugging purposes, prints the contents of the extent map.
 */
void extent_print(extent *extent);

#endif /* EXTENT_H */