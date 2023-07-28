/**************************************************************
 * Class:  CSC-415-01 Summer 2023
 * Names: Matthew Bush, Oscar Galvez, Hyok In Kwon, Jacob Lawrence
 * Student IDs: 921619696, 911813414, 922373878, 922384785
 * GitHub Name: liftcodesleep
 * Group Name: Coffee On The Rocks
 * Project: Basic File System
 *
 * File: fsInit.c
 *
 * Description: Main driver for file system assignment.
 *
 *
 **************************************************************/
#include <stdlib.h>
#include "mfs.h"

#define TEST_MAX_INDEX 20
#define MAX_CHAR 100
#define MAX_PATH 256 // default size
#define BLOCK_SIZE 512

direntry* current_working_dir;

// Text field to store absolute paths
char currentDirectory[MAX_PATH];
// Text field to store paths worked on in parsePath
char workingDirectory[MAX_PATH];


dir_and_index* parsePath(const char* givenpathname)
{
    
    if( strlen(givenpathname) == 0)
    {
        return NULL;
    }

    char * pathname = malloc(strlen(givenpathname));
    strcpy(pathname, givenpathname);


    // Token Setup
    char* token; // = malloc( 100 ); // Get 100 for the vcb
    dir_and_index* result = malloc(sizeof(dir_and_index) );
    char * saveptr;
    token = strtok_r(pathname, "/", &saveptr );



    direntry * tempEntry = malloc(BLOCK_SIZE * 4);
    
    // Check for where the path starts
    if(pathname != NULL && pathname[0] == '/')
    {
        // Start at root and loads it in
        LBAread(tempEntry, 4, 6);

    }else if(strcmp(token, "..") == 0 )
    {
        LBAread(tempEntry, 4, current_working_dir[1].extents[0].start);

    }else if(strcmp(token, ".") == 0 )
    {
        LBAread(tempEntry, 4, current_working_dir[0].extents[0].start);

    } else {
        // Catch case - Loads buffer if token is detected
        //LBAread(tempEntry, 4, 6);

        LBAread(tempEntry, 4, current_working_dir[0].extents[0].start);
    }
    

    result->dir = tempEntry;


    // parse path was root
    if(  token == NULL && pathname[0] == '/' )
    {
        result->index = 0;
        free(pathname);
        return result; 
    }


    // Go threw all values in the path
    int i;
    while(token != NULL)
    {
        result->index = -1;
        for(i = 0; i < tempEntry[0].entries; i++ )
        {
            if( strcmp( tempEntry[i].name, token ) == 0 )
            {
                result->index = i;
                break;
            }
        }

        // End if not found
        if(result->index == -1)
        {
            break;
        }


        // Update values
        LBAread(tempEntry, 4, tempEntry[i].extents[0].start);
        result->dir = tempEntry;
        token = strtok_r(NULL, "/", &saveptr );
    }

    //free(tempEntry);
    free(pathname);
    return result;
    
}


// Key directory functions
int fs_mkdir(const char *pathname, mode_t mode)
{

    dir_and_index* path = parsePath(pathname);

    int index;

    // check path with an empty value
    if(path != NULL && path->dir != NULL && path->index == -1)
    {

        index = init_dir(10, path->dir);

        // Setup to find name
        char* token;
        char* last_token = malloc(100);
        char * saveptr;
        
        // Setup for getting the name of the file
        char * copy_pathname = malloc(strlen(pathname));
        strcpy(copy_pathname, pathname);
        token = strtok_r(copy_pathname, "/", &saveptr );

        // get last value in path
        while( token != NULL  )
        {
            strcpy(last_token, token);
            token = strtok_r(NULL, "/", &saveptr);
        }
        
        // Write the new name of the file to disk
        strcpy(path->dir[index].name, last_token);
        LBAwrite(path->dir, path->dir[0].extents[0].count ,path->dir[0].extents[0].start );

        free(last_token);
        
        return 1;
    }


    return -1;
    


}

//ensure that no duplicate directory exists
int fs_rmdir(const char *pathname);

// Directory iteration functions
fdDir * fs_opendir(const char *pathname){
    dir_and_index * dai = parsePath(pathname);
    //directory to load the current directory from dir_and_index
    direntry * cD = malloc(BLOCK_SIZE * 4);
    cD = dai->dir;
    //move to the current directory using the parent and index
    loadDir(cD,dai->index);
    fdDir * fD = malloc(sizeof(fdDir));
    //fill the diriteminfo of each entry within current directory
    fD->di = malloc(sizeof(struct fs_diriteminfo) * dai->dir[dai->index].entries);
    fD->d_reclen = cD->entries;
    fD->dirEntryPosition = dai->index;
    for (int i = 0; i < cD->entries; i++){
        strcpy(fD->di[i].d_name, cD[i].name);
        fD->di[i].d_reclen = cD[i].entries;
        fD->di[i].fileType = cD[i].isFile;
        fD->di[i].location = cD[i].extents;
    }
    if (cD != NULL){
        free(cD);
        cD = NULL;
    }
    return fD;
}

struct fs_diriteminfo *fs_readdir(fdDir *dirp);
int fs_closedir(fdDir *dirp){
    if (dirp != NULL){
        if(dirp->di != NULL){
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
char * fs_getcwd(char *pathname, size_t size) {

    if (currentDirectory == NULL){
        printf("currentDirectory is empty. Fill it.\n");
        return NULL;
    }

    // If the length of the absolute pathname of the current working directory,
    // including the terminating null byte, exceeds 'size' bytes, NULL is returned.
    if (strlen(currentDirectory) > size) {
        perror("Error: The length of the pathname is larger than your buffer (size).");
        return NULL;
    }

    // If current working directory is assigned in set function, there should
    // be no need to look at a parsedPath variable or travese the array
    strcpy(pathname, currentDirectory);
    return pathname;

}
int fs_setcwd(char *pathname) {

    // If no path is given, and the home environment is not empty,
    // cd utility will navigate to home (root)
    if (strcmp(pathname, "") == 0) {
        printf("No input detected. Navigating to root.\n");
        current_working_dir = parsePath(pathname);
        strcpy(currentDirectory, current_working_dir[0].name);
        return 0;
    }

    // Absolute path - set working directory to path
    if (pathname[0] == '/') {
        strcpy(workingDirectory, pathname);
    } else if (pathname[0] == '.') {

    }

    dir_and_index* di = parsePath(pathname);

    // parsePath detected an invalid path. Error handling.
    if (di == NULL) {
        printf("cd: %s: No such file or directory\n", pathname);
        return -1;
    }

    if(di->index != -1)
    {
        current_working_dir = &di->dir[di->index];
        return 0;
    }
    return -1;

}   //linux chdir
int fs_isFile(char * filename);	//return 1 if file, 0 otherwise
int fs_isDir(char * pathname);		//return 1 if directory, 0 otherwise
int fs_delete(char* filename);	//removes a file

int fs_stat(const char *path, struct fs_stat *buf);

// Populate current_working_dir global variable to root upon initalization
void setInitialDirectory() {
    direntry *buffer = malloc(BLOCK_SIZE * 4);
    LBAread(buffer, 4, 6);
    
    // Global variable in mfs.c intialized in startup routine
    current_working_dir = buffer;
    strcpy(currentDirectory, current_working_dir[0].name);
}

// Clean up method - free global variable current_working_directory
void freeDirectory() {
    free(current_working_dir);
    current_working_dir = NULL;
}