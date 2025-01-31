/**************************************************************
 * Class:  CSC-415-01 Summer 2023
 * Names: Hyok In Kwon
 * Student IDs: 922373878
 * GitHub Name: hkwon4
 * Group Name: Coffee On The Rocks
 * Project: Basic File System
 *
 * File: dirEntry.h
 *
 * Description: Handler for directory entries. Creates directory entries,
 *              and other helper functions to facilitate modifications of
 *              directory entries.
 **************************************************************/
#ifndef DIRENTRY
#define DIRENTRY

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "b_bitmap.h"
#define BLOCK_SIZE 512

typedef struct direntry
{
  unsigned long size;
  char name[100];
  extent extents[3];
  unsigned long time_created;
  unsigned long time_last_modified;
  unsigned long time_last_accessed;
  char isFile;
  unsigned int entries;
} direntry;

int init_dir(int minEntries, direntry *parent);
void loadDir(direntry *dir, int index);
direntry *getRoot();
#endif