
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

int test_fsopendir_and_close()
{
	fdDir * test = fs_opendir("/Desktop");
	int result1 = 1;
	int result2 = 1;
	int result3 = 1;
	if (test->d_reclen > 0 ){
		printf("this is d_reclen: %d\n",test->d_reclen );
		result1 = 0;
	}
	if (test->dirEntryPosition >=0){
		printf("this is dirEntryPosition: %d\n",test->dirEntryPosition );
		result2 = 0;
	}
	if (test->di[0].d_name != NULL){
		printf("this is di[0].d_name: %s\n",test->di[0].d_name );
		result3 = 0;
	}

	if (result1 == 0 && result2 == 0 && result3 == 0){
		return 1;
	} else{
		return 0;
	}
	
}




void test_mfs()
{

	printf("\nTesting mfs functions...\n");

	test(test_mkdir,"test_mkdir");
	test(test_fsopendir_and_close, "testing fsopendir and fsclosedir ");


}