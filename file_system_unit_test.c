#include "file_system_unit_test.h"
#include "extent_unit_tests.h"
#include "parsePath.h"
#include "b_bitmap.h"



void test( int(*func)(), char* name )
{

	int passed;
	passed = func();

	if(passed)
	{
		printf("%-35s P\n", name);
	}else
	{
		printf("%-35s F\n", name);
	}

}


void extent_tests()
{
	printf("\nTesting Extents:\n");
	test( test_extent_init,"test_extent_init");
	test( test_extent_append,"test_extent_append");
	test( test_extent_at_index,"test_extent_at_index");

	test( test_extent_block_to_LBA,"test_extent_block_to_LBA");
	test( test_extent_remove_blocks,"test_extent_remove_blocks");
}



//////////////////////////////////////// PARSE PATH TESTS /////////////////////////////////

int test_valid_absolute_path() {
    const char* path = "/usr/local/bin";
    parsedPath* result = parsePath(path);
    if (result != NULL) {
        free(result);
        return 1; // Test passed
    } else {
        return 0; // Test failed
    }
}

int test_valid_relative_path() {
    const char* path = "home/user/documents";
    parsedPath* result = parsePath(path);
    if (result != NULL) {
        free(result);
        return 1; // Test passed
    } else {
        return 0; // Test failed
    }
}

int test_valid_relative_to_parent_path() {
    const char* path = "../folder1/folder2";
    parsedPath* result = parsePath(path);
    if (result != NULL) {
        free(result);
        return 1; // Test passed
    } else {
        return 0; // Test failed
    }
}

int test_empty_path() {
    const char* path = "";
    parsedPath* result = parsePath(path);
    if (result == NULL) {
        return 1; // Test passed
    } else {
        free(result);
        return 0; // Test failed
    }
}

int test_invalid_double_slash() {
    const char* path = "/usr//local";
    parsedPath* result = parsePath(path);
    if (result == NULL) {
        return 1; // Test passed
    } else {
        free(result);
        return 0; // Test failed
    }
}


void parse_path_tests()
{

	printf("\nTesting Parse Path:\n");
	test(test_valid_absolute_path,"test_valid_absolute_path");
	test(test_valid_relative_path,"test_valid_relative_path");
	test(test_valid_relative_to_parent_path,"test_valid_relative_to_parent_path");
	test(test_invalid_double_slash,"test_invalid_double_slash");
	//printf("Getting seg fault when passing empty path so commented it out");
	test(test_empty_path,"test_empty_path");
}





////////////////////////////////////////////// BIT MAP UNIT TESTS ////////////////////////////

int test_basic_allocation() {
    int num_blocks = 100;
    int min_extent_size = 10;

    extent* result = allocate_blocks(num_blocks, min_extent_size);
    if (result != NULL) {
        free(result);
        return 1; // Test passed
    } else {
        return 0; // Test failed
    }
}

int test_allocation_with_insufficient_space() {
    int invalid_num_blocks = 100000; // Set an invalid number of blocks to trigger insufficient disk space

    extent* result = allocate_blocks(invalid_num_blocks, 10);
    if (result == NULL) {
        return 1; // Test passed
    } else {
        free(result);
        return 0; // Test failed
    }
}

int test_allocation_with_minimum_extent_size() {
    int num_blocks = 100;

    extent* result = allocate_blocks(num_blocks, 1);
    if (result != NULL) {
        free(result);
        return 1; // Test passed
    } else {
        return 0; // Test failed
    }
}

void test_bit_map()
{

	printf("\nTesting Bit Map:\n");
	test(test_basic_allocation,"test_basic_allocation");
	test(test_allocation_with_insufficient_space,"test_allocation_with_insufficient_space");
	test(test_allocation_with_minimum_extent_size,"test_allocation_with_minimum_extent_size");
}

//////////////////////////////////////////////////////////////////////////////////////////






void file_system_unit_tests()
{

	printf("\n\nStarting unit tests:\n\n");


	extent_tests();
	test_bit_map();
	parse_path_tests();
	

	printf("\n\nEnding unit tests...\n\n");
}