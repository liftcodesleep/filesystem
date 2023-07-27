#include "file_system_unit_test.h"
#include "extent_unit_tests.h"
//#include "parsePath.h"
#include "b_bitmap.h"
//#include "get_and_set_dir.h"

void test(int (*func)(), char *name)
{

  int passed;
  passed = func();

  printf("%-40s", name);

  if (passed)
  {
    printf("P\n");
  }
  else
  {
    printf("F\n");
  }
}

void extent_tests()
{
  printf("\nTesting Extents:\n");
  test(test_extent_init, "test_extent_init");
  test(test_extent_append, "test_extent_append");
  test(test_extent_at_index, "test_extent_at_index");

  test(test_extent_block_to_LBA, "test_extent_block_to_LBA");
  test(test_extent_remove_blocks, "test_extent_remove_blocks");
}

//////////////////////////////////////// PARSE PATH TESTS /////////////////////////////////

int test_valid_absolute_path()
{
  const char *path = "/usr/local/bin";

  dir_and_index *result = parsePath(path);
  if (result != NULL)
  {
    free(result);
    return 1; // Test passed
  }
  else
  {
    return 0; // Test failed
  }
}

int test_valid_relative_path()
{
  const char *path = "../user/documents";

  fs_setcwd("/");

  dir_and_index *result = parsePath(path);
  if (result != NULL)
  {
    free(result);
    return 1; // Test passed
  }
  else
  {
  	free(result);
    return 0; // Test failed
  }
}

int test_valid_root_path()
{
  const char *path = "/";
  dir_and_index *result = parsePath(path);

  if ( result != NULL)
  {
    free(result);
    return 1; // Test passed
  }
  else
  {
    return 0; // Test failed
  }
}

int test_valid_relative_to_parent_path()
{
  const char *path = "../folder1/folder2";

  fs_setcwd("/");

  dir_and_index *result = parsePath(path);
  if (result != NULL)
  {
    free(result);
    return 1; // Test passed
  }
  else
  {
    return 0; // Test failed
  }
}

int test_empty_path()
{
  const char *path = "";
  dir_and_index *result = parsePath(path);
  if (result == NULL)
  {
    return 1; // Test passed
  }
  else
  {
    free(result);
    return 0; // Test failed
  }
}

int test_invalid_double_slash()
{
  const char *path = "/usr//local";
  dir_and_index *result = parsePath(path);
  if (result == NULL)
  {

    return 1; // Test passed
  }
  else
  {
    free(result);
    return 0; // Test failed
  }
}

void parse_path_tests()
{


  printf("\nTesting Parse Path:\n");
  test(test_valid_absolute_path, "test_valid_absolute_path");
  test(test_valid_relative_path, "test_valid_relative_path");
  test(test_valid_relative_to_parent_path, "test_valid_relative_to_parent_path");
  test(test_invalid_double_slash, "test_invalid_double_slash");
  //printf("Getting seg fault when passing empty path so commented it out");
  test(test_empty_path,"test_empty_path");
  test(test_valid_root_path,"test_valid_root_path");
  

}

////////////////////////////////////////////// BIT MAP UNIT TESTS ////////////////////////////

int test_basic_allocation()
{
  int num_blocks = 100;
  int min_extent_size = 10;

  extent *result = allocate_blocks(num_blocks, min_extent_size);
  if (result != NULL)
  {
    free(result);
    return 1; // Test passed
  }
  else
  {
    return 0; // Test failed
  }
}

int test_allocation_with_insufficient_space()
{
  int invalid_num_blocks = 100000; // Set an invalid number of blocks to trigger insufficient disk space

  extent *result = allocate_blocks(invalid_num_blocks, 10);
  if (result == NULL)
  {
    return 1; // Test passed
  }
  else
  {
    free(result);
    return 0; // Test failed
  }
}

int test_allocation_with_minimum_extent_size()
{
  int num_blocks = 100;

  extent *result = allocate_blocks(num_blocks, 1);
  if (result != NULL)
  {
    free(result);
    return 1; // Test passed
  }
  else
  {
    return 0; // Test failed
  }
}

int test_multiple_allocs()
{

  extent *result1 = allocate_blocks(1, 1);
  extent *result2 = allocate_blocks(1, 1);

  if (result1->start != result2->start)
  {
    return 1; // Test passed.
  }
  else
  {
    return 0; // Test failed. It allocated two diffrent blocks to the same location
  }
}

void test_bit_map()
{

  printf("\nTesting Bit Map:\n");
  test(test_basic_allocation, "test_basic_allocation");
  test(test_allocation_with_insufficient_space, "test_allocation_with_insufficient_space");
  test(test_allocation_with_minimum_extent_size, "test_allocation_with_minimum_extent_size");
  test(test_multiple_allocs, "test_multiple_allocs");
}

//////////////////////////////////////////////////////////////////////////////////////////




void file_test()
{
  direntry * newEntry = malloc(MINBLOCKSIZE * 4);
  LBAread(newEntry, 4, 6);

  // newEntry[0].entries currently doesn't populate correctly - bug with extents
  for (int i = 0; i < 12; i++) {
    printf("%s\n", newEntry[i].name);

    if (newEntry[i].isFile == 1) {
      printf("#%d isFile: %d\n", i, newEntry[i].isFile);
    }
  }

  free(newEntry);
}

void make_testdir()
{
  direntry * newEntry = malloc(MINBLOCKSIZE * 4);

  //First level
  LBAread(newEntry, 4, 6);

  strcpy(newEntry[2].name, "file1");
  strcpy(newEntry[3].name, "file2");
  strcpy(newEntry[4].name, "file3");
  strcpy(newEntry[5].name, "file4");
  strcpy(newEntry[6].name, "dir1");
  newEntry[6].isFile = 1;
  strcpy(newEntry[7].name, "file5");
  strcpy(newEntry[8].name, "file6");
  strcpy(newEntry[9].name, "file7");
  strcpy(newEntry[10].name, "file8");
  strcpy(newEntry[11].name, "file9");

  // Create second level
  newEntry[6].extents[0].start = init_dir(10, newEntry + 6);
  newEntry[6].extents[0].start = 10;
  //printf("Location: %d\n", newEntry[6].extents[0].start);

  // Write first level to block
  LBAwrite(newEntry, 4, 6);


  LBAread(newEntry, 4, 10);
  strcpy(newEntry[2].name, "newEntry1");
  strcpy(newEntry[3].name, "newEntry2");
  strcpy(newEntry[4].name, "newEntry3");
  strcpy(newEntry[5].name, "newEntry4");
  strcpy(newEntry[6].name, "newEntry5");
  strcpy(newEntry[7].name, "newEntry6");
  strcpy(newEntry[8].name, "newEntry7");
  strcpy(newEntry[9].name, "newdir2");
  newEntry[9].isFile = 1;
  strcpy(newEntry[10].name, "newEntry9");
  strcpy(newEntry[11].name, "newEntry10");
  LBAwrite(newEntry, 4, 10);

  // Create third level
  init_dir(10, newEntry + 9);
  newEntry[9].extents[0].start = 14;
  //printf("Location: %d\n", newEntry[9].extents[0].start);

  // Write second level to block
  LBAwrite(newEntry, 4, 10);

  LBAread(newEntry, 4, 14);
  strcpy(newEntry[2].name, "finalEntry1");
  strcpy(newEntry[3].name, "finalEntry2");
  strcpy(newEntry[4].name, "finalEntry3");
  strcpy(newEntry[5].name, "finalEntry4");
  strcpy(newEntry[6].name, "finalEntry5");
  strcpy(newEntry[7].name, "finalEntry6");
  strcpy(newEntry[8].name, "finalEntry7");
  strcpy(newEntry[9].name, "finalEntry8");
  strcpy(newEntry[10].name, "finalEntry9");
  strcpy(newEntry[11].name, "finalEntry10");
  LBAwrite(newEntry, 4, 14);


  free(newEntry);
}


///////////////////////////////////////////////////


void file_system_unit_tests()
{

  printf("\n\nStarting unit tests:\n\n");

  extent_tests();
  test_bit_map();
  parse_path_tests();

  make_testdir();
  file_test();

  //printf("Size of dir : %d",sizeof(direntry) );

  printf("\n\nEnding unit tests...\n\n");
}