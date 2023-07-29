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

  char *pathname = malloc(strlen(given_pathname));
  if (pathname == NULL)
  {
    printf("pathname malloc failed\n");
    return NULL;
  }
  strcpy(pathname, given_pathname);

  // Token Setup
  char *token;
  dir_and_index *result = malloc(sizeof(dir_and_index));
  if (result == NULL)
  {
    printf("result malloc failed\n");
    return NULL;
  }
  char *saveptr;
  token = strtok_r(pathname, "/", &saveptr);

  direntry *temp_entry = malloc(BLOCK_SIZE * 4);
  if (temp_entry == NULL)
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
    free(pathname);
    return result;
  }

  // Go threw all values in the path
  int i;
  while (token != NULL)
  {
    result->index = -1;
    for (i = 0; i < temp_entry[0].entries; i++)
    {
      if (strcmp(temp_entry[i].name, token) == 0)
      {
        result->index = i;
        break;
      }
    }

    // End if not found
    if (result->index == -1)
    {
      break;
    }

    // Update values
    LBAread(temp_entry, 4, temp_entry[i].extents[0].start);
    result->dir = temp_entry;
    token = strtok_r(NULL, "/", &saveptr);
  }

  // free(temp_entry);
  if (pathname != NULL)
  {
    free(pathname);
    pathname = NULL;
  }
  return result;
}

// Key directory functions
int fs_mk_internal(const char *pathname, mode_t mode, int type)
{
  printf("mk_internal: type: %d\n", type);
  dir_and_index *path = parse_path(pathname);
  int index = 0;
  // check path with an empty value
  if (path != NULL && path->dir != NULL && path->index == -1)
  {
    if (type == 1) // is_file
    {
      for (int i = 2; i < path->dir[0].entries; i++)
      {
        if (path->dir[i].name == "\0")
        {
          printf("mkfile: index: %d\n\n", index);
          index = i;
          printf("mkfile: index: %d\n\n", index);
          printf("mkfile: breakin\n");
          break;
        }
      }
    }
    else
    {
      index = init_dir(10, path->dir);
    }
    // Setup to find name
    char *token;
    char *last_token = malloc(100);
    if (last_token == NULL)
    {
      printf("last_token malloc failed\n");
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
      printf("mkfile: index: %d\n\n", index);
      strcpy(path->dir[index].name, token);
      // strcpy(last_token, token);
      token = strtok_r(NULL, "/", &saveptr);
    }
    printf("mkfile: line\n\n");
    path->dir->isFile = type;
    // Write the new name of the file to disk
    // printf("in mkdir: %d\n", path->dir[0].extents[0].start);
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
  printf("makefile: \n");
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
  free(dai->dir);
  free(dai);
}

// Directory iteration functions
fdDir *fs_opendir(const char *pathname)
{

  dir_and_index *dai = parse_path(pathname);

  // move to the current directory using the parent and index
  // loadDir(dai->dir, dai->index);
  fdDir *fD = malloc(sizeof(fdDir));
  if (fD == NULL)
  {
    printf("fD malloc failed\n");
    return NULL;
  }
  // fill the diriteminfo of each entry within current directory
  fD->di = malloc(sizeof(struct fs_diriteminfo) * dai->dir->entries);
  if (fD->di == NULL)
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
  if (dirp != NULL)
  {
    if (dirp->di != NULL)
    {
      free(dirp->di);
      dirp->di = NULL;
    }
    free(dirp);
    dirp = NULL;
    return 1;
  }
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
        strcat(workingDirectory, "/");
      }

      strcat(workingDirectory, pathname);
    }
  }
  else
  {
    strcpy(workingDirectory, currentDirectory);
    strcat(workingDirectory, "/");
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

    return 0;
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
  if (di->dir != NULL)
  {
    free(di->dir);
    di->dir = NULL;
  }
  if (di != NULL)
  {
    free(di);
    di = NULL;
  }
  return 1;
}

// Populate current_working_dir global variable to root upon initalization
int set_initial_directory()
{
  current_working_dir = malloc(BLOCK_SIZE * 4);
  if (current_working_dir == NULL)
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
  if (current_working_dir != NULL)
  {
    free(current_working_dir);
    current_working_dir = NULL;
  }
}

int fs_delete(char *filename)
{
  return -1;
}