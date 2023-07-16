
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


int test_extent_append()
{
  extent *extent = make_test_extent();

  // Append 5 blocks starting from block number 10
  unsigned int count1 = extent_append(extent, 10, 5);
  unsigned int count2 = extent_append(extent, 15, 6);
  unsigned int count3 = extent_append(extent, 30, 1);
  unsigned int count4 = extent_append(extent, 40, 1);

  // Should be in the 2nd extent table
  unsigned int count5 = extent_append(extent, 50, 2);
  unsigned int count6 = extent_append(extent, 60, 3);

  for(int i = 4; i < 70; i++)
  {
    extent_append(extent, 70 + i+i, i);
  }
  

  //extent_print(extent);

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



