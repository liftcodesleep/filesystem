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

int make_2nd_extent_table(extent* extent, uint last_extent_size);
unsigned int append_second_extent(int extent_loc,  uint block_number, uint count);

int make_3rd_extent_table(extent* extent, uint last_extent_size);
unsigned int append_third_extent(int extent_loc,  uint block_number, uint count);

/////////////////////////////////////////////////////////////////




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
      if(blocks_added == 0) break;
      return blocks_added;

    }else if(extent[current_index].count == SENTINAL_THIRD_EXTENT)
    {
      extent_type = 3;
      unsigned int blocks_added;
      blocks_added = append_third_extent(extent[current_index].start, block_number, count);
      if(blocks_added == 0) break;
      return blocks_added;
    }
  }

  // The first extent table is full!
  if(extent_type == 1)
  {
    make_2nd_extent_table(extent,EXTENT_SIZE-1);
    unsigned int blocks_added;
    blocks_added = append_second_extent(extent[current_index-1].start, block_number, count);
    return blocks_added;
  }


  // The second extent table is full!
  if(extent_type == 2)
  {
    make_3rd_extent_table(extent,EXTENT_SIZE-1);
    unsigned int blocks_added;
    blocks_added = append_third_extent(extent[current_index].start, block_number, count);
    return blocks_added;
  }

  return 0; // No more space for additional extents
}


pextent extent_at_index(pextent extent, uint i)
{
  uint extent_index = 0;

  // 
  for (extent_index = 0; i < EXTENT_SIZE; extent_index++)
  {
    if(extent_index == i && extent[extent_index].count != SENTINAL_SECOND_EXTENT)
    {
      return extent+extent_index;
    }
  }

  extent_index--;
  if(extent[extent_index].count = SENTINAL_SECOND_EXTENT)
  {
    pextent second_extent = malloc(MINBLOCKSIZE);
    
    LBAread(second_extent, 1, extent[extent_index].start);
    
    for (int j = 0; j < 64; j++)
    {
      if(extent_index == i)
      {
        return second_extent + j;
      }
      
      if(second_extent[j].count == 0)
      {
        return NULL;
      }
      extent_index++;
    }

  }

  return NULL;

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

// Prints all the values in the extent
void extent_print(extent *extent)
{
  printf("\n\n  Extent Table\nLocation    Count\n");
  for (int i = 0; i < EXTENT_SIZE; i++)
  {
    printf("%05d        %05x\n", extent[i].start, extent[i].count);
  }
  
  if(extent[2].count == SENTINAL_SECOND_EXTENT)
  {
    pextent second_extent = malloc(MINBLOCKSIZE);
    
    LBAread(second_extent, 1, extent[EXTENT_SIZE-1].start);
    
    printf("-- Second Extent --\n");
    for (int i = 0; i < 64; i++)
    {
      printf("%05d        %05d\n", second_extent[i].start, second_extent[i].count);
      if(second_extent[i].count == 0)
      {
        break;
      }
    }
    free(second_extent);

  }else if(extent[EXTENT_SIZE-1].count == SENTINAL_THIRD_EXTENT)
  {
    uint* third_extent = malloc(MINBLOCKSIZE);

    LBAread(third_extent, 1, extent[EXTENT_SIZE-1].start);

    for(int i = 0; i < 128; i++)
    {
      
      pextent second_extent = malloc(MINBLOCKSIZE);
      LBAread(second_extent, 1, third_extent[i]);
      if(third_extent[i] == 0)
      {
        free(second_extent);
        break;
      }
      printf("- Second Extent %d-\n", third_extent[i]);
      for (int i = 0; i < 64; i++)
      {
        printf("%05d        %05d\n", second_extent[i].start, second_extent[i].count);
        if(second_extent[i].count == 0)
        {
          break;
        }
      }

      if(third_extent[i] == 0)
      {
        break;
      }
      free(second_extent);
    }

    printf("- Third Extent -\n");
    for(int i = 0; i < 128; i++)
    {
      printf("   %09d \n", third_extent[i]);
      if(third_extent[i] == 0)
      {
        break;
      }
    }
    free(third_extent);

  }



  printf("\n\n");
  


}


//////////////////////////// Private Functions //////////////////////////////



/*
* Makes a new 2nd extent with the first value as the extent passed
* Returns the block where the table is saved
*/
int make_2nd_extent_table(extent* extent, uint last_extent_size)
{

  pextent p_loc = allocate_blocks(1, 1);

  pextent new_table = calloc(64,sizeof(extent));
  new_table[0].start = extent[last_extent_size].start;
  new_table[0].count = extent[last_extent_size].count;

  extent[last_extent_size].start = p_loc[0].start;
  extent[last_extent_size].count = SENTINAL_SECOND_EXTENT;

  LBAwrite(new_table, 1, p_loc[0].start);
  free(p_loc);

  return p_loc[0].start;
}

unsigned int append_second_extent(int extent_loc,  uint block_number, uint count)
{
  // Read in the 2nd extent from disk
  pextent extent = malloc(MINBLOCKSIZE);
  uint blocks_read;
  blocks_read = LBAread(extent, 1, extent_loc);

  // Going threw 2nd extent
  for(int current_index = 0; current_index < 64; current_index++)
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

  free(extent);
  // Second Extent is full
  return 0; 
  
}



/*
* Makes a new 3nd extent with the first value as the location passed
* Returns the block where the table is saved
*/
int make_3rd_extent_table(extent* extent, uint last_extent_size)
{

  //pextent p_loc = allocate_blocks(1, 1);
  pextent p_loc = malloc(sizeof(extent));
  p_loc[0].start = 5000;
  p_loc[0].count = 1;
  //printf("SAVING THIRD TABLE AT %d", p_loc[0].start );

  uint* new_table = calloc(128,sizeof(uint));
  new_table[0] = extent[last_extent_size].start;
  
  extent[last_extent_size].start = p_loc[0].start;
  extent[last_extent_size].count = SENTINAL_THIRD_EXTENT;

  LBAwrite(new_table, 1, p_loc[0].start);
  free(p_loc);

  return p_loc[0].start;
}


unsigned int append_third_extent(int extent_loc,  uint block_number, uint count)
{
  // Read in the 3rd extent from disk
  uint* extent_3 = malloc(MINBLOCKSIZE);
  uint blocks_read;
  blocks_read = LBAread(extent_3, 1, extent_loc);

  uint saved_blocks = 0;
  for(int i = 0; i < 128; i++)
  {

    if(extent_3[i] == 0)
    {
      //printf("AAAAAAAAAAAAa %d\n", count);
      //pextent p_loc = allocate_blocks(1,1);
      pextent p_loc = malloc(sizeof(extent));
      p_loc[0].start = 7000;
      p_loc[0].count = 1;

      pextent new_2_extent = calloc(64,sizeof(extent));
      new_2_extent[0].start = block_number;
      new_2_extent[0].count = count;

      extent_3[i] = p_loc[0].start;

      LBAwrite(extent_3,1,extent_loc);
      LBAwrite(new_2_extent,1, p_loc[0].start);

      free(p_loc);
      free(new_2_extent);
      free(extent_3);
      return count;
    }

    //printf("AAAAAAAAAAAAa %d %d\n", count, i);
    saved_blocks = append_second_extent(extent_3[i], block_number, count);
    if(saved_blocks != 0)
    {

      free(extent_3);
      return saved_blocks;
    }
  }
  
  free(extent_3);
  // Third Extent is full
  return 0; 
  
}