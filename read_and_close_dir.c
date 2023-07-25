

#include "mfs.h"


struct fs_diriteminfo *fs_readdir(fdDir *dirp)
{
	if(dirp->dirEntryPosition >= dirp->d_reclen )
	{
		return NULL;
	}
	
	return di+dirp->dirEntryPosition++;
}


int fs_closedir(fdDir *dirp)
{
	dirp->dirEntryPosition = 0;
}