/**************************************************************
 * Class:  CSC-415-01 Spring 2023
 * Name: Jacob Lawrence
 * Student ID: 922384785
 * Group Name: Coffee on the Rocks
 * Project: Basic File System
 *
 * File: b_bitmap.h
 *
 * Description: This file allows its user to initialize and interact with 
 * a bitmap to manage the free space of their system. It provides functions
 * to load, allocate, and free blocks.
 **************************************************************/
#include "b_io.h"
#ifndef BITMAP_H
#define BITMAP_H

typedef struct extent
{
  int start;
  int count;
} extent, *pextent;

// make space return first free block num
int init_free_space(int block_count, int block_size);

// get space
int load_free_space(int block_count, int block_size);

// use this to size things to fit our extents
extent *allocate_blocks(int blocks_required, int min_per_extent);

// frees an extent's worth
void release_blocks(int start, int count);

#endif
