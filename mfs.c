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
#define MAX_CHAR 100
#define MAX_PATH 256 // default size
#define BLOCK_SIZE 512

char currentDirectory[MAX_PATH];
char * bufferBlock; // Load blocks of memory to read and traverse

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
//OBSOLETE WILL DELETE LATER****************************
//parsedPath ppath;
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
//returns pointer to parsedPath struct or NULL if it fails
//future plan to validate paths using 
parsedPath* parsePath(const char* pathname){
    //allocate space
    parsedPath* ppath = (parsedPath*)malloc(sizeof(char*) * TEST_MAX_INDEX * MAX_CHAR + sizeof(int) *4 + 1);
    //init ppath member variables
    ppath->pathSize = 0;
    ppath->absPath = 0;
    ppath->relPath = 0;
    ppath->parPath = 0;
    //save pointer for strtok_r
    char * tokP;
    //copy of pathname to be passed in strtok_r
    char * copy = malloc(strlen(pathname));
    //check if relative path or absolute
    if (pathname[0] == '/'){
        ppath->absPath = 1;
    }
    strcpy(copy, pathname);
    //first token
    char* token1 = strtok_r(copy, "/", &tokP);
    if (strcmp(token1, "\0") == 0){ //Invalid path
        printf("INVALID PATH\n");
        return NULL;
    }
    else if (strcmp(token1, ".") == 0){ //check if its relative path
        ppath->relPath = 1; 
    } else if (strcmp(token1, "..") == 0){ //check if its relative to parent
        ppath->parPath = 1;
    } else {
        //allocate space for each entry
        ppath->pathArray[ppath->pathSize] = malloc(strlen(token1)+1);
        strcpy(ppath->pathArray[ppath->pathSize], token1);
        ppath->pathSize++;
    }
    //token2 will be used to tokenize remaining path levels
    char * token2 = strtok_r(NULL, "/", &tokP); //get second token
    if(token2 == NULL){ //END of path
        //use findIndex to find the index of directory entry
    } else {
        //add tokenized path sections to pathArray
        while (token2 != NULL && ppath->pathSize < TEST_MAX_INDEX){
            //allocate space for each entry
            ppath->pathArray[ppath->pathSize] = malloc(strlen(token2)+1);
            strcpy(ppath->pathArray[ppath->pathSize], token2);
            ppath->pathSize++;
            token2 = strtok_r(NULL, "/", &tokP);
        }
    }
    return ppath;
}

//helper function to free memory of parsedPath
void freePath(parsedPath* ppath){
    for (int i = 0; i < ppath->pathSize; i++){
        free(ppath->pathArray[i]);
    }
    free(ppath);
}

//helper function for returning index of directory based on name of directory entry
int findEntry(direntry startDir, char* token){
    return 0;
}

//reads a direntry to memory
//to be used for various path validations
void loadDir(char* dir){

}





// Key directory functions
int fs_mkdir(const char *pathname, mode_t mode);
//ensure that no duplicate directory exists
int fs_rmdir(const char *pathname);

// Directory iteration functions
fdDir * fs_opendir(const char *pathname);
struct fs_diriteminfo *fs_readdir(fdDir *dirp);
int fs_closedir(fdDir *dirp);

// Misc directory functions
char * fs_getcwd(char *pathname, size_t size) {

    // Value passed in for size is 4096 (DIRMAX_LEN)
    //
    // If NULL is returned, the size of the pointer is too small
    // to contain the whole path - manpage of getcwd() linux function
    if (strlen(pathname) > size) {
        printf("Invalid - path is longer than max path size.\n");
        return NULL;
    }

    // If current working directory is assigned in set function, there should
    // be no need to look at a parsedPath variable or travese the array
    strcpy(pathname, currentDirectory);
    return pathname;

}
int fs_setcwd(char *pathname) {

    //fsInIt - root initalization
    if (strcmp(pathname, "/root") == 0) {
        strcpy(currentDirectory,"/root");
        return 0;
    }

    // 4096 is equal to the DIRMAX_LEN value defined in mfs.c - Hardcoded at the moment
    if (strlen(currentDirectory) + strlen(pathname) < 4096) {
        strcpy(currentDirectory, "/root"); // Set to root to start traversing
    }

    int validateResult = -1; // Set to -1 - Needs to be tested first
    //validatePath(pathname); Considering validatePath returns a valid address
    if (validateResult == -1) {
        printf("Path was determined to be invalid. Exiting.\n");
        return -1;
    }

    // If pathname was validated, set as current working directory.
    strcpy(currentDirectory, pathname);
    return 0;

    // TODO: Determine how to traverse direntries for files

    // Load root directory into bufferBlock

    // clear previous working directory
    // strcpy(currentDirectory, "/0");
    
    // set new working directory
    
}   //linux chdir
int fs_isFile(char * filename);	//return 1 if file, 0 otherwise
int fs_isDir(char * pathname);		//return 1 if directory, 0 otherwise
int fs_delete(char* filename);	//removes a file

int fs_stat(const char *path, struct fs_stat *buf);

// Currently accounting to read root directory which occupies four blocks - For testing
void allocateBuffer() {
    bufferBlock = malloc(BLOCK_SIZE * 4);
}