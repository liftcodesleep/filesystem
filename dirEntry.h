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

#include "b_bitmap.h"


typedef struct direntry 
{
    unsigned long size;
    char name[100];
    struct extent extents[3];
    unsigned long time_created;
    unsigned long time_last_modified;
    unsigned long time_last_accessed;
    
} direntry;

int init_dir(int minEntries, direntry * parent);