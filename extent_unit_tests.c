
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

  unsigned int total = 5+6+1+1+2+3;
  unsigned int test_total = count1 +count2+count3+count4+count5+count6;
  for(int i = 4; i < 70; i++)
  {
    total += extent_append(extent, 70 + i+i, 1);
    test_total++;
  }

  //extent_print(extent);

  // Verify the result
  if (count1 == 5 && count2 == 6 && count3 == 1 && total == test_total)
  {
    return 1;
  }
  else
  {
    return 0;
  }

  free(extent);
}


int test_extent_at_index()
{

  extent *extent = make_test_extent();

  // Append 5 blocks starting from block number 10
  
  for(int i = 10; i < 85; i++)
  {
    extent_append(extent, 10+i+i+1, i);
  }
  
  
  pextent test1 = NULL;
  pextent test2 = NULL;
  pextent test3 = NULL;

  test1 = extent_at_index(extent, 1);
  test2 = extent_at_index(extent, 3);
  test3 = extent_at_index(extent, 65);

  
  if( test1 != NULL && test1->count == 11 
      && test2 != NULL && test2->count == 13 
      && test3 != NULL && test3->count == 75 
    )
  {
    return 1;
  }else
  {
    return 0;
  }
  
  

  return 0;

}

int test_extent_block_to_LBA()
{
  extent *extent = make_test_extent();

  
  for(int i = 0; i < 65; i++)
  {
    extent_append(extent, 10+i+1, 1);
  }
  
  unsigned int test1 = extent_block_to_LBA(extent, 1);
  //unsigned int test2 = extent_block_to_LBA(extent, 50);

  //printf("TEST: %d\n", test1);

  
  if( test1  == 12 
      //&& test2 == 13 
      //&& test3  == 75 
    )
  {
    return 1;
  }else
  {
    return 0;
  }
  
  

  return 0;

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









