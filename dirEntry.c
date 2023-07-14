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
#include "extent.h"
#include "b_bitmap.h"
#include "fsLow.h"

#define BLOCK_SIZE 512

typedef struct dirent 
{
    unsigned long size;
    char name[100];
    struct Extent extents[3];
    unsigned long time_created;
    unsigned long time_last_modified;
    unsigned long time_last_accessed;
} dirent;


int init_dir(int minEntries, dirent * parent){
    //minimum bytes needed to fit amount of directory entries
    int minBytes = minEntries * sizeof(dirent);
    //minimum bytes needed to allocate memory
    int blocksNeeded = blockCount(minBytes,BLOCK_SIZE);
    int bytesAlloc = blocksNeeded*BLOCK_SIZE;
    //actual number of entries
    int actualNEntries = bytesAlloc/sizeof(dirent);
    //allocate bytes for new directory
    dirent * newDir = malloc(bytesAlloc);
    //entry 0 is . and entry 1 is ..
    for (int i = 2; i < actualNEntries ; i++){
        //directory entry initialize as unused
        newDir[i].name = "\0";
        newDir[i].time_created = 0;
        newDir[i].time_last_modified = 0;
        newDir[i].time_last_accessed = 0;
    }
    //make root
    strcpy(newDir[0].name, ".");
    newDir[0].size = bytesAlloc;
    strcpy(newDir[1].name, "..");
    Extent *e = allocate_blocks(blocksNeeded,blocksNeeded);
    newDir[0].extents[0] = e;
    free(e);
    //setup root directory if parent is NULL
    if (parent == NULL) {
        newDir[1].size = newDir[0].size;
        newDir[1].extents[0] = newDir[0].extents[0];
    }
    //write to disc
    LBAwrite(newDir, blocksNeeded, newDir[0].extents[0].start_loc);
    return newDir[0].extents[0].start_loc;
    
}

//function to do quick maths on number of blocks needed for n bytes
int blockCount(int n, int divider){
    return (n+divider-1)/divider;
}