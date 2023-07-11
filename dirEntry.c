/**************************************************************
* Class:  CSC-415-0# Fall 2021
* Names: Hyok In Kwon
* Student IDs:
* GitHub Name:
* Group Name:
* Project: Basic File System
*
* File: fsInit.c
*
* Description: Main driver for file system assignment.
*
* This file is where you will start and initialize your system
*
**************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>			
#include <string.h>
#include <time.h>

#include "dirEntry.h"

#define BLOCK_SIZE 512

typedef struct dirent 
{
    unsigned long size;
    char name[100];
    unsigned int extents[3][2];
    unsigned long time_created;
    unsigned long time_last_modified;
    unsigned long time_last_accessed;
} dirent;


void init_dir(int minEntries, dirent * parent){
    //minimum bytes needed to fit amount of directory entries
    int minBytes = minEntries * sizeof(dirent);
    //minimum bytes needed to allocate memory
    int bytesAlloc = blockCount(minBytes,BLOCK_SIZE)*BLOCK_SIZE;
    //actual number of entries
    int actualNEntries = bytesAlloc/sizeof(dirent);
    //allocate bytes for new directory
    dirent * newDir = malloc(bytesAlloc);
    //entry 0 is . and entry 1 is ..
    for (int i = 2; i < actualNEntries ; i++){
        //directory entry initialize as unused 
        newDir[i].name = "\0";
        //the rest of the values here;
    }
    strcpy(newDir[0].name, ".");
    newDir[0].size = bytesAlloc; //why?? 
    strcpy(newDir[1].name, "..");
    //initialize locations using extents here
    //first allocate space
}

//function to do quick maths on number of blocks needed for n bytes
int blockCount(int n, int divider){
    return (n+divider-1)/divider;
}