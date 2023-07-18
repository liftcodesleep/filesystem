#include "file_system_unit_test.h"
#include "extent_unit_tests.h"



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

void file_system_unit_tests()
{

	printf("\n\nStarting unit tests:\n\n");


	extent_tests();


	printf("\n\nEnding unit tests...\n\n");
}