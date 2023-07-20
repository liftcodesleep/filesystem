/**************************************************************
 * Class:  CSC-415-01 Summer 2023
 * Names: Hyok In Kwon
 * Student IDs: 922373878
 * GitHub Name: hkwon4
 * Group Name: Coffee On The Rocks
 * Project: Basic File System
 *
 * File: parsePath.h
 *
 * Description: Handler for directory entries. Creates directory entries,
 *              and other helper functions to facilitate modifications of
 *              directory entries.
 *
 * 
 **************************************************************/


#ifndef PARSEPATH
#define PARSEPATH

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define TEST_MAX_INDEX 20


//struct to hold each level of the path
typedef struct parsedPath{
    //array to hold each level
    char **pathArray;
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

parsedPath* parsePath(const char* pathname);
void freePath(parsedPath* ppath);



#endif
