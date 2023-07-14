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

int init_dir(int minEntries, dirent * parent);