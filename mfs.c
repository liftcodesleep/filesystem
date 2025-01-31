/**************************************************************
 * Class:  CSC-415-01 Summer 2023
 * Names: Matthew Bush, Oscar Galvez, Hyok In Kwon, Jacob Lawrence
 * Student IDs: 921619696, 911813414, 922373878, 922384785
 * GitHub Name: liftcodesleep
 * Group Name: Coffee On The Rocks
 * Project: Basic File System
 *
 * File: mfs.c
 *
 * Description: file system interface functions for taking paths
 *              and handling information needed from shell
 *              commands
 *
 **************************************************************/
#include <stdlib.h>
#include "mfs.h"

#define TEST_MAX_INDEX 20
#define MAX_CHAR 100
#define MAX_PATH 256 // default size
#define BLOCK_SIZE 512

direntry *current_working_dir;

// Text field to store absolute paths
char currentDirectory[MAX_PATH];
// Text field to store paths worked on in parse_path
char workingDirectory[MAX_PATH];

dir_and_index *parse_path(const char *given_pathname)
{
  // Error handling - Empty path passed in
  if (strlen(given_pathname) == 0)
  {
    return NULL;
  }

  char *pathname;
  if (malloc_wrap(strlen(given_pathname) + 1, (void **)&pathname, "pathname")) // VALGRIND ERROR +1
  {
    return NULL; // Malloc failed. Reporting failure
  }
  strcpy(pathname, given_pathname);

  // Token Setup
  char *token;
  dir_and_index *result;
  if (malloc_wrap(sizeof(dir_and_index), (void **)&result, "result"))
  {
    return NULL; // Malloc failed. Reporting failure
  }
  char *saveptr;
  token = strtok_r(pathname, "/", &saveptr);

  direntry *temp_entry;
  if (malloc_wrap(BLOCK_SIZE * 4, (void **)&temp_entry, "temp_entry"))
  {
    printf("temp_entry malloc failed\n");
    return NULL; // Malloc failed. Reporting failure
  }

  // Check for where the path starts
  if (pathname != NULL && pathname[0] == '/')
  {
    // Start at root and loads it in
    LBAread(temp_entry, 4, 6);
  }
  else if (strcmp(token, "..") == 0)
  {
    // Start at parent
    LBAread(temp_entry, 4, current_working_dir[1].extents[0].start);
  }
  else if (strcmp(token, ".") == 0)
  {
    // Start at self
    LBAread(temp_entry, 4, current_working_dir[0].extents[0].start);
  }
  else
  {
    // Catch case - Loads buffer if token is detected
    LBAread(temp_entry, 4, current_working_dir[0].extents[0].start);
  }

  result->dir = temp_entry;

  // parse path was root - Returning values equivalent to root
  if (token == NULL && pathname[0] == '/')
  {
    result->index = 0;
    FREE(pathname);
    return result;
  }

  int i;
  // Go through all values in the path
  while (token != NULL)
  {
    // Reports failure if value doesn't get changed in loop
    result->index = -1;
    for (i = 0; i < temp_entry[0].entries; i++)
    {
      // Success - Match found - index assigned to return to calling function 
      if (strcmp(temp_entry[i].name, token) == 0)
      {
        result->index = i;
        break;
      }
    }

    // Failure to find match in current directory - Exit loop
    if (result->index == -1)
    {
      break;
    }

    //  Update values - Moving to next directory
    LBAread(temp_entry, 4, temp_entry[i].extents[0].start);
    result->dir = temp_entry;
    token = strtok_r(NULL, "/", &saveptr);
  }

  FREE(pathname);

  return result;
}

// Key directory functions
int fs_mk_internal(const char *pathname, mode_t mode, int type)
{
  dir_and_index *path = parse_path(pathname);
  int index = 0;

  // Check path with an empty value
  if (path != NULL && path->dir != NULL && path->index == -1)
  {
    if (type == 1) // File flag
    {
      int i;

      // Find first empty spot within directory
      for (i = 2; i < path->dir[0].entries; i++)
      {
        if (strcmp(path->dir[i].name, "\0") == 0)
        {
          index = i; // Set index of empty spot in directory
          break;
        }
      }
      // Full directory has been traversed and all slots full
      if (i == path->dir[0].entries)
      {
        printf("Directory is full!\n");
        return -1;
      }

      // Assign relevant variables to file entry
      path->dir[index].isFile = type;
      path->dir[index].extents[0].start = path->dir[0].extents[0].start;
      path->dir[index].extents[0].count = 1;
    }
    else // Create a new directory - File flag not detected
    {
      index = init_dir(10, path->dir);
    }

    // Setup to find name
    char *token;
    char *last_token;
    if (malloc_wrap(100, (void **)&last_token, "last_token"))
    {
      return -1; // Malloc failed. Reporting failure
    }

    // Setup for getting the name of the file
    char *saveptr;
    char *copy_pathname = strdup(pathname);
    if (copy_pathname == NULL) // Error handling
    {
      printf("copy_pathname malloc failed\n");
      return -1;
    }
    token = strtok_r(copy_pathname, "/", &saveptr);

    // get last value in path
    while (token != NULL)
    {
      strcpy(path->dir[index].name, token);
      token = strtok_r(NULL, "/", &saveptr);
    }

    // Write the new name of the file to disk
    LBAwrite(path->dir, path->dir[0].extents[0].count, path->dir[0].extents[0].start);
    FREE(last_token);
    FREE(copy_pathname);

    return 0;
  }

  return -1;
}

int fs_mkdir(const char *pathname, mode_t mode)
{
  return fs_mk_internal(pathname, mode, 0);
}

int fs_mkfil(const char *pathname, mode_t mode)
{
  return fs_mk_internal(pathname, mode, 1);
}

// ensure that no duplicate directory exists
int fs_rmdir(const char *pathname)
{

  dir_and_index *dai = parse_path(pathname);
  // parse_path reported a failure - Aborting function
  if (dai == NULL || dai->dir == NULL || dai->index == -1)
  {
    return -1;
  }

  // check if there is anything within the directory
  // if there are used directory entries, remove fails
  // Accounts for self '.' and root '..' occupying first two entries
  for (int i = 2; i < dai->dir->entries; i++)
  {
    // Entry within directory detected - Abort removal of directory
    if (strcmp(dai->dir[i].name, "\0") != 0)
    {
      printf("ERROR (REMOVE FAILED): THIS DIRECTORY CONTAINS ITEMS\n");
      return -1;
    }
  }

  // Load into the parent directory - Using parent values to map size and block location
  LBAread(dai->dir, dai->dir[1].extents[0].count, dai->dir[1].extents[0].start);

  strcpy(dai->dir[dai->index].name, "\0");
  extent_remove_blocks(dai->dir[dai->index].extents, 0, 0); /// 0,0 are place holders

  // Write changes back into memory
  LBAwrite(dai->dir, dai->dir[0].extents[0].count, dai->dir[0].extents[0].start);

  // Free memory
  FREE(dai->dir);
  FREE(dai);
}

// Directory iteration functions
fdDir *fs_opendir(const char *pathname)
{

  dir_and_index *dai = parse_path(pathname);

  // move to the current directory using the parent and index
  // loadDir(dai->dir, dai->index);
  fdDir *fD;
  if (malloc_wrap(sizeof(fdDir), (void **)&fD, "fD"))
  {
    return NULL;
  }
  // fill the diriteminfo of each entry within current directory
  if (malloc_wrap(sizeof(struct fs_diriteminfo) * dai->dir->entries,
                  (void **)&fD->di, "fD->di"))
  {
    printf("fD->di malloc failed\n");
    return NULL; // Malloc failed. Reporting failure
  }

  fD->d_reclen = dai->dir->entries;
  fD->dirEntryPosition = 0;

  // Populate file descriptor array (fcb_array)
  for (int i = 0; i < dai->dir->entries; i++)
  {
    strcpy(fD->di[i].d_name, dai->dir[i].name);
    fD->di[i].d_reclen = dai->dir[i].size;
    fD->di[i].fileType = dai->dir[i].isFile;
    fD->di[i].location = dai->dir[i].extents;
  }

  return fD;
}

struct fs_diriteminfo *fs_readdir(fdDir *dirp)
{

  while (dirp->dirEntryPosition < dirp->d_reclen)
  {
    dirp->dirEntryPosition++;

    // Emptry directory position detected - exiting function
    if (((dirp->di) + (dirp->dirEntryPosition - 1))->d_name[0] != '\0')
    {
      return (dirp->di) + (dirp->dirEntryPosition - 1);
    }
  }
  
  // No emptry directory positon
  return NULL;
}

// Wrapper function to free allocated resourced for a directory
int fs_closedir(fdDir *dirp)
{
  FREE(dirp->di);
  FREE(dirp);
  return 0;
}

// Misc directory functions
char *fs_getcwd(char *pathname, size_t size)
{
  if (currentDirectory == NULL)
  {
    printf("currentDirectory is empty. Fill it.\n");
    return NULL;
  }
  else if (strcmp(currentDirectory, ".") == 0)
  {
    return "/";
  }

  // If the length of the absolute pathname of the current working directory,
  // including the terminating null byte, exceeds 'size' bytes, NULL is returned.
  if (strlen(currentDirectory) > size)
  {
    perror("Error: The length of the pathname is larger than your buffer (size).");
    return NULL;
  }

  // If current working directory is assigned in set function, there should
  // be no need to look at a parsedPath variable or travese the array
  strcpy(pathname, currentDirectory);
  return pathname;
}
int fs_setcwd(char *pathname)
{
  // Refresh buffer to ensure most current changes are reflected and accesssible
  LBAread(current_working_dir, 4, current_working_dir[0].extents[0].start);

  // If no path is given or root is entered, and the home environment is not empty,
  // cd utility will navigate to home (root)
  if ((strcmp(pathname, "") == 0) || (pathname[0] == '/' && strlen(pathname) == 1))
  {
    LBAread(current_working_dir, 4, 6);
    strcpy(currentDirectory, current_working_dir[0].name);
    return 0;
  }

  // cd '.' - No futher action required
  if (strcmp(pathname, ".") == 0)
  {
    return 0;
  }

  // Absolute path - set working directory to path
  // Path starting with '/' must begin at root to be valid
  if (pathname[0] == '/')
  {
    strcpy(workingDirectory, pathname);
  }
  else if (pathname[0] == '.')
  {
    strcpy(workingDirectory, pathname);

    // If pathname does not begin with a /, set workingDirectory to
    // the string formed by the concatentation of the value currentDirectory +
    // a '/' character if it does not end with a slash + and pathname
    if (workingDirectory[0] != '/')
    {
      strcpy(workingDirectory, currentDirectory);

      // Detect if forward slash is not at the end of the current directory
      if (currentDirectory[strlen(currentDirectory) - 1] != '/')
      {
        strcat(workingDirectory, "/");
      }

      strcat(workingDirectory, pathname);
    }
  }
  else
  {
    strcpy(workingDirectory, currentDirectory);

    // Detect if forward slash is not at the end of the current directory
    if (currentDirectory[strlen(currentDirectory) - 1] != '/')
    {
      strcat(workingDirectory, "/");
    }

    strcat(workingDirectory, pathname);
  }

  dir_and_index *di = parse_path(workingDirectory);

  // parse_path detected an invalid path. Error handling.
  if (di == NULL)
  {
    return -1;
  }

  if (di->index != -1)
  {
    // Load the new working directory into the global variable buffer
    current_working_dir = di->dir;
    loadDir(current_working_dir, di->index);

    // Attempting to cd into a file - Reverse action and abort
    if (current_working_dir[di->index].isFile == 1)
    {
      if (current_working_dir[0].extents[0].start != current_working_dir[di->index].extents[0].start)
      {
        loadDir(current_working_dir, current_working_dir[1].extents[0].start);
      }

      return (-1);
    }

    // For each dot-dot component - If there is a preceding component
    // that is neither root not dot-dot, then:
    // The preceding component, all slash characters seperating the preceding
    // component from dot-dot, dot-dot, and all slash characters seperating dot-dot
    // from the following component (if any) shall be deleted.
    // - From Linux manpage for cd

    char tempDirectory[256] = "";
    char previousToken[30] = "";

    char *tok = strtok(workingDirectory, "/");
    strcpy(previousToken, tok);
    tok = strtok(NULL, "/");

    // Two tokens used to piece together the absolute path of the destination
    // The best implementation ended up being several if conditions for the
    // circumstances defined in the manpage for linux cd command
    // previousToken is used to step backwards if '..' is detected at the end.
    // tempDirectory used as working directory for this algorithm
    while (tok != NULL)
    {
      // If '..' in path, ignore and look at next file
      if (strcmp(tok, "..") == 0)
      {
        strcpy(previousToken, tok);
        strtok(NULL, "/");
        break;
      }

      // If token is '.' and root has not already been copied in
      else if (strcmp(previousToken, ".") == 0 && strlen(tempDirectory) == 0)
      {
        strcat(tempDirectory, "/");
        strcpy(previousToken, tok);
        strtok(NULL, "/");
        break;
      }

      // Insert inital '/' if not detected
      if (strlen(tempDirectory) == 0)
      {
        strcat(tempDirectory, "/");
      }
      strcat(tempDirectory, previousToken);
      strcat(tempDirectory, "/");
      strcpy(previousToken, tok);
      tok = strtok(NULL, "/");
    }

    // Ensure parent and self are not being detected
    if (strcmp(previousToken, ".") != 0 && strcmp(previousToken, "..") != 0)
    {
      if (strlen(tempDirectory) == 1)
      {
        strcat(tempDirectory, previousToken);
      }
      else
      {
        strcat(tempDirectory, previousToken);
        strcat(tempDirectory, "/");
      }
    }

    // Error handling - If somehow an empty value got through the start of the
    // function and through parsePath. Assign '/' to indicate root
    if (strcmp(tempDirectory, "") == 0)
    {
      strcpy(currentDirectory, "/");
    }
    else
    {
      strcpy(currentDirectory, tempDirectory);
    }

    return 0;
  }

  return (-1);

} // linux chdir

// return 1 if file, 0 otherwise
int fs_is_file(char *filename)
{
  dir_and_index *di = parse_path(filename);
  return di->dir[di->index].isFile == 1;
}

// return 1 if directory, 0 otherwise
int fs_is_dir(char *pathname)
{
  dir_and_index *di = parse_path(pathname);
  return di->dir[di->index].isFile == 0;
}

// Function to delete the selected file
int fs_delete(char *filename)
{
  dir_and_index *dai = parse_path(filename);

  // Variable to catch return values of function
  int err = 0;

  // Error handling - If filename passed in is a directory
  if (dai->dir[dai->index].isFile == 0)
  {
    printf("Attempting to remove a directory. fs_delete failed.\n");
    return (-1);
  }

  // Error handling - parse_path failed - Not a valid path
  if (dai == NULL || dai->dir == NULL || dai->index == -1)
  {
    printf("Invalid path. rm failed.\n");
    return (-1);
  }

  LBAread(dai->dir, dai->dir[0].extents[0].count, dai->dir[0].extents[0].start);

  // Delete file - Starting with basic metadeta
  // Wipe name - Scrub each bit name occupied with '\0'
  int length = strlen(dai->dir[dai->index].name);
  for (int i = 0; i < length; i++)
  {
    dai->dir[dai->index].name[i] = '\0';
  }

  dai->dir[dai->index].size = 0;
  dai->dir[dai->index].time_created = 0;
  dai->dir[dai->index].time_last_modified = 0;
  dai->dir[dai->index].time_last_accessed = 0;
  dai->dir[dai->index].isFile = 0;

  err = extent_remove_blocks(dai->dir[dai->index].extents, 0, 0); /// 0,0 are place holders
  if (err == -1)
  {
    printf("extent_remove_blocks failed in fs_delete. Aborting.\n");
    return (-1);
  }

  // Scrub location
  dai->dir[dai->index].extents[0].count = 0;
  dai->dir[dai->index].extents[0].start = 0;


  LBAwrite(dai->dir, dai->dir[0].extents[0].count, dai->dir[0].extents[0].start);

  // Free memory
  FREE(dai->dir);

  return (0);

} // removes a file

int fs_stat(const char *path, struct fs_stat *buf)
{
  dir_and_index *di = parse_path(path);
  buf->st_size = di->dir->size;
  buf->st_createtime = di->dir->time_created;
  buf->st_accesstime = di->dir->time_last_accessed;
  buf->st_modtime = di->dir->time_last_modified;
  buf->st_blocks = extent_size(di->dir->extents);
  buf->st_blksize = MINBLOCKSIZE;
  FREE(di->dir);
  FREE(di);
  return 1;
}

// Populate current_working_dir global variable to root upon initalization
int set_initial_directory()
{
  // TODO make idempotent
  if (malloc_wrap(BLOCK_SIZE * 4, (void **)&current_working_dir,
                  "current_working_dir"))
  {
    printf("current_working_dir malloc failed\n");
    return -1;
  }
  LBAread(current_working_dir, 4, 6);
  // Global variable in mfs.c intialized in startup routine
  strcpy(currentDirectory, current_working_dir[0].name);
}

// Clean up method - free global variable current_working_directory
void free_directory()
{
  FREE(current_working_dir);
}