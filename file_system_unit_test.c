#include "file_system_unit_test.h"
#include "extent_unit_tests.h"
#include "parsePath.h"


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

	test( test_extent_init,"test_extent_init");
	test( test_extent_append,"test_extent_append");
	test( test_extent_at_index,"test_extent_at_index");

	test( test_extent_block_to_LBA,"test_extent_block_to_LBA");
	test( test_extent_remove_blocks,"test_extent_remove_blocks");
}


void test_parsePath() {
    // Test 1: Valid absolute path
    const char* path1 = "/usr/local/bin";
    parsedPath* result1 = parsePath(path1);
    if (result1 != NULL) {
        printf("Test 1 (Valid absolute path): Passed\n");
    } else {
        printf("Test 1 (Valid absolute path): Failed\n");
    }
    free(result1);

    // Test 2: Valid relative path
    const char* path2 = "home/user/documents";
    parsedPath* result2 = parsePath(path2);
    if (result2 != NULL) {
        printf("Test 2 (Valid relative path): Passed\n");
    } else {
        printf("Test 2 (Valid relative path): Failed\n");
    }
    free(result2);

    // Test 3: Valid relative to parent path
    const char* path3 = "../folder1/folder2";
    parsedPath* result3 = parsePath(path3);
    if (result3 != NULL) {
        printf("Test 3 (Valid relative to parent path): Passed\n");
    } else {
        printf("Test 3 (Valid relative to parent path): Failed\n");
    }
    free(result3);

    // Test 4: Empty path (Invalid)
    const char* path4 = "";
    parsedPath* result4 = parsePath(path4);
    if (result4 == NULL) {
        printf("Test 4 (Empty path): Passed\n");
    } else {
        printf("Test 4 (Empty path): Failed\n");
    }
    free(result4);

    // Test 5: Invalid path (contains "//")
    const char* path5 = "/usr//local";
    parsedPath* result5 = parsePath(path5);
    if (result5 == NULL) {
        printf("Test 5 (Invalid path with double slashes): Passed\n");
    } else {
        printf("Test 5 (Invalid path with double slashes): Failed\n");
    }
    free(result5);
}






void file_system_unit_tests()
{

	printf("\n\nStarting unit tests:\n\n");


	extent_tests();
	test_parsePath();


	printf("\n\nEnding unit tests...\n\n");
}