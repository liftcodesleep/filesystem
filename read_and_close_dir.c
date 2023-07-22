

#include "mfs.h"


struct fs_diriteminfo *fs_readdir(fdDir *dirp)
{

	fs_diriteminfo* info = malloc(sizeof(fs_diriteminfo));



	info->d_reclen = 0;
	info->fileType = 0;
	info->d_name = "Test";

	
	

}


int fs_closedir(fdDir *dirp)
{


}