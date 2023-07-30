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
#define SENTINAL_SECOND_EXTENT 0xffffffff
#define SENTINAL_THIRD_EXTENT 0xfffffffe

typedef unsigned int uint;

////// Change extent to an array of struct
// Initializes the extent to its initial state.
void extent_init(extent *extent);
/*
 * Adds more blocks to the extent. Also merges the extent
 * Returns:
 *   - The number of blocks successfully added.
 */
unsigned int extent_append(extent *extent, uint block_number, uint count);
// Takes the local block number and returns where it is stored in the LBA
// Returns: The LBA from the block passed
unsigned int extent_block_to_LBA(extent *extent, unsigned int local_block_number);
/*
 * gets the extents at an index
 * MUST FREE THE RETURNED EXTENT!!!
 * Return NULL if bad value or an empty extent if at the end
 */
pextent extent_at_index(pextent extent, uint i);
/*
 * Removes blocks from the extent.
 * Returns:
 *   - The number of blocks successfully removed.
 */
unsigned int extent_remove_blocks(extent *extent, uint block_number, uint count);
// Returns the blocks in the extent
unsigned int extent_size(extent *extent);
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
int extent_print(extent *extent);

#endif /* EXTENT_H */