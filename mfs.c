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

#include "mfs.h"

#define TEST_MAX_INDEX 20

//struct to hold each level of the path
typedef struct parsedPath{
    //array to hold each level
    char *pathArray[TEST_MAX_INDEX];
    //count of how many levels
    //used for iterative purposes
    int pathSize;
    //bool for absolute path
    int absPath;
    //bool for relative path 
    int relPath;
    //bool for relative to parent
    int parPath;
}parsedPath;

//parsed path container
parsedPath ppath;
//indicator if ppath has been initialized
int p_init = 0;

//initializer for ppath array member
void init_parsedPath(){
    if (p_init == 1 ){
        return;
    }
    for (int i = 0; i < TEST_MAX_INDEX ; i++){
        ppath->pathArray[i] = NULL;
    }
    ppath->pathSize = 0;
    ppath->absPath = 0;
    ppath->relPath = 0;
    ppath->parPath = 0;
    p_init = 1;

}

//returns based on successful parsing
//returns index of direntry or -1 if it does not exist
//future plan to validate paths using 
int parsePath(const char* pathname){
    //initialize parsedPath object if needed
    init_parsedPath()
    //reset parsedPath values for next call
    ppath->pathSize = 0;
    ppath->absPath = 0;
    ppath->relPath = 0;
    ppath->parPath = 0;
    //int index = 0;
    //pointer for strtok_r
    char * tokP;
    //copy of pathname to be passed in strtok_r
    char * copy;
    //check if relative path or absolute
    if (strcmp(pathname[0], "/") == 0){
        ppath->absPath = 1;
    }
    strcpy(copy, pathname);
    //first token
    char* token1 = strtok_r(copy, "/", &tokP);
    if (token == NULL){ //Invalid path
        return -1;
    } else if (strcmp(token, ".") == 0){ //check if its relative path
        ppath->relPath = 1;
    } else if (strcmp(token, "..") == 0){ //check if its relative to parent
        ppath->parPath = 1;
    } else {
        strcpy(ppath->pathArray[ppath->pathSize], token);
        ppath->pathSize++;
    }
    char * token2 = strtok_r(NULL, "/");
    if(token2 == NULL){ //END of path
        //use findIndex to find the index of directory entry
    } else {
        //add tokenized path sections to pathArray
        while (token2 != NULL){
            strcpy(ppath->pathArray[ppath->pathSize], token2);
            ppath->pathSize++;
            token2 = strtok_r(NULL, "/", &tokP);
        }
    }
    return ppath->pathSize;

}

//helper function for returning index of directory based on name of directory entry
int findEntry(direntry startDir, char* token){
    return 0;
}



// Key directory functions
int fs_mkdir(const char *pathname, mode_t mode);
int fs_rmdir(const char *pathname);

// Directory iteration functions
fdDir * fs_opendir(const char *pathname);
struct fs_diriteminfo *fs_readdir(fdDir *dirp);
int fs_closedir(fdDir *dirp);

// Misc directory functions
char * fs_getcwd(char *pathname, size_t size);
int fs_setcwd(char *pathname);   //linux chdir
int fs_isFile(char * filename);	//return 1 if file, 0 otherwise
int fs_isDir(char * pathname);		//return 1 if directory, 0 otherwise
int fs_delete(char* filename);	//removes a file

int fs_stat(const char *path, struct fs_stat *buf);