
#include <stdlib.h>
#include "mfs.h"
#include "file_system_unit_test.h"



int test_mkdir()
{

	fs_setcwd("/");

	int result = fs_mkdir("/Desktop",0);

	int set_result = fs_setcwd("/Desktop");
	
	if(result == 1 && set_result == 0)
	{
		return 1;
	}else
	{
		return 0;
	}



	return 1;
}




void test_mfs()
{

	printf("\nTesting mfs functions...\n");

	test(test_mkdir,"test_mkdir");


}