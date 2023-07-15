
#include <stdlib.h>
#include "extent.h"

///////////// UNIT TESTS //////////////////////////

extent *make_test_extent()
{
  extent *extent = malloc(sizeof(extent) * 3);

  extent_init(extent);

  return extent;
}

int test_extent_init()
{
  extent *extent = make_test_extent();

  // Check if each extent element is initialized correctly
  int i;
  int failed = 0;
  for (i = 0; i < EXTENT_SIZE; i++)
  {
    if (extent[i].start != 0 || extent[i].count != 0)
    {
      failed = 1;
      break;
    }
  }

  if (failed)
  {
    return 0;
  }
  else
  {
    return 1;
  }

  free(extent);
}

int test_extent_get_block_num()
{
  extent *extent = make_test_extent();

  // Append 5 blocks starting from block number 10
  extent_append(extent, 10, 5);

  // Test extent_get_block_num for different indices
  unsigned int blockNum1 = extent_get_block_num(extent, 0);
  unsigned int blockNum2 = extent_get_block_num(extent, 2);
  unsigned int blockNum3 = extent_get_block_num(extent, 4);

  // char question[] = ( "Block number at index 0: %u\n", blockNum1);
  // char question[] = ( "Block number at index 2: %u\n", blockNum2);
  // char question[] = ( "Block number at index 4: %u\n", blockNum3);

  // Verify the results
  if (blockNum1 == 10 && blockNum2 == 12 && blockNum3 == 14)
  {
    return 1;
  }
  else
  {
    return 0;
  }

  free(extent);
}

int test_extent_append()
{
  extent *extent = make_test_extent();

  // Append 5 blocks starting from block number 10
  unsigned int count1 = extent_append(extent, 10, 5);
  unsigned int count2 = extent_append(extent, 15, 6);
  unsigned int count3 = extent_append(extent, 30, 1);


  // Verify the result
  if (count1 == 5 && count2 == 6 && count3 == 1)
  {
    return 1;
  }
  else
  {
    return 0;
  }

  free(extent);
}

int test_extent_remove_blocks()
{
  extent extent[EXTENT_SIZE];
  extent_init(extent);

  // Append 5 blocks starting from block number 10
  extent_append(extent, 10, 5);

  // Remove 3 blocks starting from block number 12
  unsigned int count = extent_remove_blocks(extent, 12, 3);


  // char question[] = ( "Number of blocks removed: %u\n", count);

  // Verify the result
  if (count == 3)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int test_extent_get_total_blocks()
{
  extent extent[EXTENT_SIZE];
  extent_init(extent);

  // Append 5 blocks starting from block number 10
  extent_append(extent, 10, 5);

  // Get the total number of blocks in the extent
  unsigned int totalBlocks = extent_get_total_blocks(extent);


  // char question[] = ( "Total number of blocks in extent: %u\n", totalBlocks);

  // Verify the result
  if (totalBlocks == 5)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

