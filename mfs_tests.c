
#include <stdlib.h>
#include "mfs.h"
#include "file_system_unit_test.h"



int test_mkdir()
{
	return 1;
}




void test_mfs()
{

	printf("\nTesting mfs functions...\n");

	test(test_mkdir,"test_mkdir");


}