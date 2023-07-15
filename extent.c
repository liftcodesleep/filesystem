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
#include "extent.h"

// Prints all the values in the extent
void extent_print(extent *extent)
{
  printf("Location    Count\n");
  for (int i = 0; i < EXTENT_SIZE; i++)
  {
    printf("%05d        %05d\n", extent[i].start, extent[i].count);
  }
}

// Get the block number of based in the index in the extent
int extent_get_block_num(extent *extent, uint index)
{

  int block_index = 0;
  for (int i = 0; i < EXTENT_SIZE; i++)
  {

    if (extent[i].count + block_index > index)
    {
      return extent[i].start + index - block_index;
    }
    block_index += extent[i].count;
  }

  return -1;
}


// Adds more blocks to the extent and merges if necessary
unsigned int extent_append(extent *extent, uint block_number, uint count)
{

  uint current_index;
  for (current_index = 0; current_index < EXTENT_SIZE; current_index++)
  {

    if (extent[current_index].count == 0)
    {

      if (current_index > 0 && extent[current_index - 1].count + extent[current_index - 1].start == block_number)
      {
        extent[current_index - 1].count += count; // Merge with the previous extent
        return count;
      }

      extent[current_index].start = block_number; // Start a new extent
      extent[current_index].count = count;
      return count; 
    }
  }

  return 0; // No more space for additional extents
}

// Removes blocks from the extent
unsigned int extent_remove_blocks(extent *extent, uint block_number, uint count)
{
  // TODO:
  return 0;
}


// Initializes the extent to its initial state
void extent_init(extent *extent)
{

  for (uint i = 0; i < EXTENT_SIZE; i++)
  {
    extent[i].start = 0; // Set the start location to 0
    extent[i].count = 0; // Set the block count to 0
  }
}

// Returns the total number of blocks in the extent
unsigned int extent_get_total_blocks(extent *extent)
{
  int block_total = 0;
  for (int i = 0; i < EXTENT_SIZE; i++)
  {
    block_total += extent[i].count; // Accumulate the block counts
  }

  return block_total;
}

// Frees the memory allocated for the extent
void extent_free(extent *extent)
{

  free(extent);
}

/*
* Makes a new 2nd extent with the first value as the extent passed
* Returns the block where the table is saved
*/
int make_2nd_extent_table(extent* extent){

	pextent p_loc = allocate_blocks(1, 1);

	LBAwrite(extent, 1, p_loc->start);


	return p_loc->start;
}

/*
* Return a pointer to an extent at the block loation given
*/
extent* load_2nd_extent_table(uint block_number){

	pextent table = malloc( sizeof(extent)*64 );

	LBAread(table, 1, block_number);

	return table;


}


/*
* Makes a new 3nd extent with the first value as the location passed
* Returns the block where the table is saved
*/
int make_3rd_extent_table(uint location){

	pextent p_loc = allocate_blocks(1, 1);

	uint* location_buffer = &location;

	LBAwrite(location_buffer, 1, p_loc->start);


	return p_loc->start;
}

/*
* Return a pointer to an extent at the block loation given
*/
extent* load_3rd_extent_table(uint block_number){

	extent* table = malloc( sizeof(block_number)*128 );

	LBAread(table, 1, block_number);

	return table;

}
