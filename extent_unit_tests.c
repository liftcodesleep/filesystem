
#include <stdlib.h>
#include "extent.h"

///////////// UNIT TESTS //////////////////////////

extent *make_test_extent()
{
  extent *extent;
  if (malloc_wrap(sizeof(extent) * 3, (void **)&extent, "extent"))
  {
    return NULL;
  }
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
  return 1;
  FREE(extent);
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
  unsigned int total = 5 + 6 + 1 + 1 + 2 + 3;
  unsigned int test_total = count1 + count2 + count3 + count4 + count5 + count6;
  for (int i = 4; i < 70; i++)
  {
    total += extent_append(extent, 70 + i + i, 1);
    test_total++;
  }
  // extent_print(extent);
  // Verify the result
  if (count1 == 5 && count2 == 6 && count3 == 1 && total == test_total)
  {
    return 1;
  }
  return 0;
  FREE(extent);
}

int test_extent_at_index()
{
  extent *extent = make_test_extent();
  // Append 5 blocks starting from block number 10
  for (int i = 10; i < 85; i++)
  {
    extent_append(extent, 10 + i + i + 1, i);
  }
  pextent test1 = NULL;
  pextent test2 = NULL;
  pextent test3 = NULL;
  test1 = extent_at_index(extent, 1);
  test2 = extent_at_index(extent, 3);
  test3 = extent_at_index(extent, 65);
  if (test1 != NULL && test1->count == 11 && test2 != NULL && test2->count == 13 && test3 != NULL && test3->count == 75)
  {
    return 1;
  }
  return 0;
}

int test_extent_block_to_LBA()
{
  extent *extent = make_test_extent();
  for (int i = 0; i < 70; i++)
  {
    extent_append(extent, 10 + i * 4, i % 3 + 1);
  }
  // extent_print(extent);
  unsigned int test1 = extent_block_to_LBA(extent, 0);
  unsigned int true_location1 = 10;
  unsigned int test2 = extent_block_to_LBA(extent, 5);
  unsigned int true_location2 = 20;
  // printf("True: %d Guess: %d\n",  true_location, test1);
  if (test1 == true_location1)
  //&& test2 == true_location2
  {
    return 1;
  }
  return 0;
}
int test_extent_remove_blocks()
{
  extent extent[EXTENT_SIZE];
  extent_init(extent);

  // Append 5 blocks starting from block number 10
  extent_append(extent, 10, 5);

  unsigned int count = extent_remove_blocks(extent, 12, 3);

  // Verify the result
  if (extent[0].start == 0 && extent[0].count == 0)
  {
    return 1;
  }
  return 0;
}

int test_extent_size()
{
  extent extent[EXTENT_SIZE];
  extent_init(extent);
  // Append 5 blocks starting from block number 10
  extent_append(extent, 10, 1);
  extent_append(extent, 100, 3);
  extent_append(extent, 150, 5);
  unsigned int size = extent_size(extent);
  // Verify the result
  if (size == 9)
  {
    return 1;
  }
  return 0;
}
