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


///////////////// Private Functions /////////////////////////////
unsigned int append_second_extent(int extent_loc,  uint block_number, uint count);
int make_2nd_extent_table(extent* extent, uint last_extent_size);
extent* load_2nd_extent_table(uint block_number);
int make_3rd_extent_table(uint location);
extent* load_3rd_extent_table(uint block_number);



// Prints all the values in the extent
void extent_print(extent *extent)
{
  printf("\n\n  Extent Table\nLocation    Count\n");
  for (int i = 0; i < EXTENT_SIZE; i++)
  {
    printf("%05d        %05d\n", extent[i].start, extent[i].count);
  }

  if(extent[EXTENT_SIZE-1].count = SENTINAL_SECOND_EXTENT)
  {
    pextent second_extent = malloc(MINBLOCKSIZE);
    
    LBAread(second_extent, 1, extent[EXTENT_SIZE-1].start);
    printf("- Second Extent -\n");
    for (int i = 0; i < 64; i++)
    {
      printf("%05d        %05d\n", second_extent[i].start, second_extent[i].count);
      if(second_extent[i].count == 0)
      {
        break;
      }
    }

  }

  printf("\n\n");
  

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

  
  uint extent_type = 1;
  uint current_index;
  for (current_index = 0; current_index < EXTENT_SIZE; current_index++)
  {
    // Found an open extent
    if (extent[current_index].count == 0  )
    {
      // Merge with the last extent
      if (current_index > 0 && extent[current_index - 1].count + extent[current_index - 1].start == block_number)
      {
        extent[current_index - 1].count += count; 
        return count;
      }

      // Start a new extent
      extent[current_index].start = block_number; 
      extent[current_index].count = count;
      return count; 
    }

    // Found a second extent table
    if(extent[current_index].count == SENTINAL_SECOND_EXTENT)
    {
      
      extent_type = 2;
      unsigned int blocks_added;
      blocks_added = append_second_extent(extent[current_index].start, block_number, count);

      return blocks_added;
    }
  }


  //printf("\nAAAAAAAAAAAAAAAAAAaa %d \n", extent[current_index].start);
  // The first 3 extents are full!
  if(extent_type == 1)
  {

    make_2nd_extent_table(extent,EXTENT_SIZE-1);
    unsigned int blocks_added;
    blocks_added = append_second_extent(extent[current_index-1].start, block_number, count);

    return blocks_added;
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

//////////////////////////// Private Functions //////////////////////////////




unsigned int append_second_extent(int extent_loc,  uint block_number, uint count)
{
  // Read in the 2nd extent from disk
  pextent extent = malloc(MINBLOCKSIZE);
  uint blocks_read;
  blocks_read = LBAread(extent, 1, extent_loc);

  // Going threw 2nd extent
  for(int current_index = 0; current_index < 128; current_index++)
  {

    // Found an open extent
    if (extent[current_index].count == 0  )
    {
      if (current_index > 0 && extent[current_index - 1].count + extent[current_index - 1].start == block_number)
      {
        // Merge with the previous extent
        extent[current_index - 1].count += count; 
        LBAwrite(extent, 1, extent_loc);
        free(extent);
        return count;
      }

      // Start a new extent
      extent[current_index].start = block_number; 
      extent[current_index].count = count;
      LBAwrite(extent, 1, extent_loc);
      free(extent);
      return count; 
    }
  }

  // Second Extent is full
  return 0; 
  
}


/*
* Makes a new 2nd extent with the first value as the extent passed
* Returns the block where the table is saved
*/
int make_2nd_extent_table(extent* extent, uint last_extent_size)
{

  // pextent p_loc = allocate_blocks(1, 1);
  pextent p_loc = malloc(sizeof(extent));
  p_loc[0].start = 123;
  p_loc[0].count = 1;



  pextent new_table = malloc(MINBLOCKSIZE);
  printf( "From make table: %d", extent[last_extent_size].start );
  new_table[0].start = extent[last_extent_size].start;
  new_table[0].count = extent[last_extent_size].count;

  extent[last_extent_size].start = p_loc[0].start;
  extent[last_extent_size].count = SENTINAL_SECOND_EXTENT;
  

	LBAwrite(new_table, 1, p_loc[0].start);
  free(p_loc);


	return p_loc[0].start;
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
