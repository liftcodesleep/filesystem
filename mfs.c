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

  if (strlen(given_pathname) == 0)
  {
    return NULL;
  }

  char *pathname;
  if (malloc_wrap(strlen(given_pathname), (void *)&pathname, "pathname"))
  {
    return NULL;
  }
  strcpy(pathname, given_pathname);

  // Token Setup
  char *token;
  dir_and_index *result;
  if (malloc_wrap(sizeof(dir_and_index), (void *)&result, "result"))
  {
    return NULL;
  }
  char *saveptr;
  token = strtok_r(pathname, "/", &saveptr);

  direntry *temp_entry;
  if (malloc_wrap(BLOCK_SIZE * 4, (void *)&temp_entry, "temp_entry"))
  {
    printf("temp_entry malloc failed\n");
    return NULL;
  }

  // Check for where the path starts
  if (pathname != NULL && pathname[0] == '/')
  {
    // Start at root and loads it in
    LBAread(temp_entry, 4, 6);
  }
  else if (strcmp(token, "..") == 0)
  {
    LBAread(temp_entry, 4, current_working_dir[1].extents[0].start);
  }
  else if (strcmp(token, ".") == 0)
  {
    LBAread(temp_entry, 4, current_working_dir[0].extents[0].start);
  }
  else
  {
    // Catch case - Loads buffer if token is detected
    // LBAread(temp_entry, 4, 6);
    LBAread(temp_entry, 4, current_working_dir[0].extents[0].start);
  }

  result->dir = temp_entry;

  // parse path was root
  if (token == NULL && pathname[0] == '/')
  {
    result->index = 0;
    FREE(pathname);
    return result;
  }

  // Go through all values in the path
  int i;
  while (token != NULL)
  {
    result->index = -1;
    for (i = 0; i < temp_entry[0].entries; i++)
    {

      if (strcmp(temp_entry[i].name, token) == 0)
      {
        printf("in parse path: %s\n",temp_entry[i].name);
        result->index = i;
        break;
      }
    }

    // End if not found
    if (result->index == -1)
    {
      break;
    }
    printf("in parse path index: %d\n",result->index);
    // Update values
    LBAread(temp_entry, 4, temp_entry[i].extents[0].start);
    result->dir = temp_entry;
    token = strtok_r(NULL, "/", &saveptr);
  }

  FREE(pathname);

  printf("FINAL RETURN: %d\n",result->index);
  return result;
}

// Key directory functions
int fs_mk_internal(const char *pathname, mode_t mode, int type)
{
  dir_and_index *path = parse_path(pathname);
  int index = 0;
  // check path with an empty value
  if (path != NULL && path->dir != NULL && path->index == -1)
  {
    if (type == 1) // is_file
    {
      int i;
      for (i = 2; i < path->dir[0].entries; i++)
      {

        if (strcmp(path->dir[i].name, "\0") == 0)
        {
          index = i;
          break;
        }
      }
      if (i == path->dir[0].entries)
      {
        printf("Directory is full!\n");
        return -1;
      }

        path->dir[index].isFile = type;
        path->dir[index].extents[0].start = path->dir[0].extents[0].start;
    }
    else
    {
      index = init_dir(10, path->dir);
    }
    // Setup to find name
    char *token;
    char *last_token;
    if (malloc_wrap(100, (void *)&last_token, "last_token"))
    {
      return -1;
    }
    char *saveptr;
    // Setup for getting the name of the file
    char *copy_pathname = strdup(pathname);
    if (copy_pathname == NULL)
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
    //printf("in mkdir: %d\n", path->dir[0].extents[0].start);
    LBAwrite(path->dir, path->dir[0].extents[0].count, path->dir[0].extents[0].start);
    FREE(last_token);
    FREE(copy_pathname);

    return 1;
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
  if (dai == NULL || dai->dir == NULL || dai->index == -1)
  {
    return -1;
  }

  LBAread(dai->dir, dai->dir[1].extents[0].count, dai->dir[1].extents[0].start);

  strcpy(dai->dir[dai->index].name, "\0");
  extent_remove_blocks(dai->dir[dai->index].extents, 0, 0); /// 0,0 are place holders

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
  if (malloc_wrap(sizeof(fdDir), (void *)&fD, "fD"))
  {
    return NULL;
  }
  // fill the diriteminfo of each entry within current directory
  if (malloc_wrap(sizeof(struct fs_diriteminfo) * dai->dir->entries,
                  (void *)&fD->di, "fD->di"))
  {
    printf("fD->di malloc failed\n");
    return NULL;
  }
  fD->d_reclen = dai->dir->entries;
  fD->dirEntryPosition = 0;
  for (int i = 0; i < dai->dir->entries; i++)
  {

    strcpy(fD->di[i].d_name, dai->dir[i].name);
    fD->di[i].d_reclen = dai->dir[i].entries;
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
    if (((dirp->di) + (dirp->dirEntryPosition - 1))->d_name[0] != '\0')
    {
      return (dirp->di) + (dirp->dirEntryPosition - 1);
    }
  }

  return NULL;
}

int fs_closedir(fdDir *dirp)
{
  FREE(dirp->di);
  FREE(dirp);
  return 0;
}

// Misc directory functions
char *fs_getcwd(char *pathname, size_t size)
{
  printf("Getcwd: Testing for: %s\n", currentDirectory);
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
  LBAread(current_working_dir, 4, current_working_dir[0].extents[0].start);
  printf("CHECK PERMISSION: %d\n", current_working_dir[2].isFile);
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

      if (currentDirectory[strlen(currentDirectory) - 1] != '/')
      {
        printf("triggered\n");
        strcat(workingDirectory, "/");
      }

      strcat(workingDirectory, pathname);
    }
  }
  else
  {
    strcpy(workingDirectory, currentDirectory);

    if (currentDirectory[strlen(currentDirectory) - 1] != '/')
    {
      printf("triggered\n");
      strcat(workingDirectory, "/");
    }

    strcat(workingDirectory, pathname);
  }

  printf("Being fed into parse_path: %s\n", workingDirectory);
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
    printf("What is here? Name: %s flag: %d index: %d\n", di->dir->name, di->dir->isFile, di->index);

    loadDir(current_working_dir, di->index);
    printf("Verify where we loaded into: %d\n", current_working_dir[0].extents[0].start);

    // Attempting to cd into a file - Reverse action and abort
    if (current_working_dir[di->index].isFile == 1) {
      printf("Verify index: #%d\n", di->index);
      if (current_working_dir[0].extents[0].start != current_working_dir[di->index].extents[0].start) {
          printf("Verify we are loading into the parent upon failure: %d\n", current_working_dir[1].extents[0].start);
          loadDir(current_working_dir, current_working_dir[1].extents[0].start);
      }
      printf("Attempting to change into a file. fuckouttahere. \n");
      return -1;
    }

    // 8) For each dot-dot component - If there is a preceding component
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

    while (tok != NULL)
    {
      if (strcmp(tok, "..") == 0)
      {
        strcpy(previousToken, tok);
        strtok(NULL, "/");
        break;
        // If token is '.' and root has not already been copied in
      }
      else if (strcmp(previousToken, ".") == 0 && strlen(tempDirectory) == 0)
      {
        strcat(tempDirectory, "/");
        strcpy(previousToken, tok);
        strtok(NULL, "/");
        break;
      }
      if (strlen(tempDirectory) == 0)
      {
        strcat(tempDirectory, "/");
      }
      strcat(tempDirectory, previousToken);
      strcat(tempDirectory, "/");
      strcpy(previousToken, tok);
      tok = strtok(NULL, "/");
    }
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
    if (strcmp(tempDirectory, "") == 0)
    {
      strcpy(currentDirectory, "/");
    }
    else
    {
      strcpy(currentDirectory, tempDirectory);
    }

    printf("Returned: %s\n", currentDirectory);
    return 0;

    // Double token method contained too many bugs - Let use an array


  }
  return -1;

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
int fs_delete(char *filename); // removes a file

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
  if (malloc_wrap(BLOCK_SIZE * 4, (void *)&current_working_dir,
                  "current_working_dir"))
  {
    printf("current_working_dir malloc failed\n");
    return -1;
  }
  LBAread(current_working_dir, 4, 6);
  // Global variable in mfs.c intialized in startup routine
  // current_working_dir = buffer;
  strcpy(currentDirectory, current_working_dir[0].name);
}

// Clean up method - free global variable current_working_directory
void free_directory()
{
  FREE(current_working_dir);
}

int fs_delete(char *filename)
{
  return -1;
}