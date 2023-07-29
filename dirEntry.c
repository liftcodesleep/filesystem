/**************************************************************
 * Class:  CSC-415-01 Summer 2023
 * Names: Hyok In Kwon
 * Student IDs: 922373878
 * GitHub Name: hkwon4
 * Group Name: Coffee On The Rocks
 * Project: Basic File System
 *
 * File: dirEntry.c
 *
 * Description: Handler for directory entries. Creates directory entries,
 *              and other helper functions to facilitate modifications of
 *              directory entries.
 *
 *
 **************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dirEntry.h"
#include "extent.h"
#include "fsLow.h"

// a helper function to get the proper block count
int blockCount(int n, int divider);

// function to initialize directory entries
// takes a value to set the minimum number of entries to be created
// takes a directory entry to be set as the parent
// when parent is null, sets up the root directory
// returns the value of the first block of the first extent
int init_dir(int minEntries, direntry *parent)
{

  // minimum bytes needed to fit amount of directory entries
  int minBytes = minEntries * sizeof(direntry);
  // minimum bytes needed to allocate memory
  int blocksNeeded = blockCount(minBytes, BLOCK_SIZE);
  int bytesAlloc = blocksNeeded * BLOCK_SIZE;
  // actual number of entries
  int actualNEntries = bytesAlloc / sizeof(direntry);
  // allocate bytes for new directory
  direntry *newDir = malloc(bytesAlloc);
  if (newDir == NULL)
  {
    printf("newDir malloc failed\n");
    return -1;
  }
  // entry 0 is . and entry 1 is ..
  for (int i = 2; i < actualNEntries; i++)
  {
    // directory entry initialize as unused
    strcpy(newDir[i].name, "\0");
    newDir[i].time_created = 0;
    newDir[i].time_last_modified = 0;
    newDir[i].time_last_accessed = 0;
    newDir[i].isFile = 0;
    newDir[i].entries = 0;
  }
  // make root
  // strcpy(newDir[0].name, ".");
  // NEED TO CHECK IF THIS IS OK**************************************************
  newDir[0].size = sizeof(direntry);
  newDir[0].entries = actualNEntries;
  // strcpy(newDir[1].name, "..");
  extent *e = allocate_blocks(blocksNeeded, blocksNeeded);
  newDir[0].extents[0] = e[0];
  newDir[0].extents[1] = e[1];
  newDir[0].extents[2] = e[2];
  newDir[1].entries = newDir[0].entries;
  FREE(e);

  int free_entry = 0;
  // setup root directory if parent is NULL
  if (parent == NULL)
  {
    strcpy(newDir[0].name, ".");
    strcpy(newDir[1].name, "..");

    newDir[1].size = newDir[0].size;
    newDir[1].extents[0] = newDir[0].extents[0];
    newDir[1].extents[1] = newDir[0].extents[1];
    newDir[1].extents[2] = newDir[2].extents[2];
  }
  else
  {

    // Find a free entry and save its position - Never enters the loop

    for (free_entry = 2; free_entry < parent->entries; free_entry++)
    {
      if (strcmp(parent[free_entry].name, "\0") == 0)
      {
        break;
      }
    }

    if (free_entry == parent->entries)
    {
      printf("found no free direntries");
      return (-1);
    }

    // strcpy(newDir[0].name, "NEW Value");
    // strcpy(newDir[1].name, parent->name);
    strcpy(newDir[0].name, ".");
    strcpy(newDir[1].name, "..");
    parent[free_entry] = newDir[0];

    // Write parent block location in extent - TESTING
    newDir[1].extents[0].start = parent[0].extents[0].start;
    newDir[1].extents[0].count = parent[0].extents[0].count;

    LBAwrite(parent, parent[0].extents[0].count, parent[0].extents[0].start);
  }

  // write to disc
  LBAwrite(newDir, blocksNeeded, newDir[0].extents[0].start);

  return free_entry;
}
// function to load new directories
// must have one initialized in advance
void loadDir(direntry *dir, int index)
{
  LBAread(dir, dir[index].extents[0].count, dir[index].extents[0].start);
}
// function to initialize the root directory
direntry *getRoot()
{
  direntry *root = malloc(BLOCK_SIZE * 4);
  if (root == NULL)
  {
    printf("Root malloc failed\n");
    return NULL;
  }
  // printf("size of root: %d\n", sizeof(direntry));
  LBAread(root, 4, 7);
  return root;
}

// function to do quick maths on number of blocks needed for n bytes
int blockCount(int n, int divider)
{
  return (n + divider - 1) / divider;
}
