/**************************************************************
 * Class:  CSC-415-01 Spring 2023
 * Name: Jacob Lawrence
 * Student ID: 922384785
 * Group Name: Coffee on the Rocks
 * Project: Basic File System
 *
 * File: b_bitmap.c
 *
 * Description: This file allows its user to initialize and interact with
 * a bitmap to manage the free space of their system. It provides functions
 * to load, allocate, and free blocks.
 **************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> // for malloc
#include "b_bitmap.h"
#include "fsLow.h"

enum on_masks
{
  bit0 = 0x00000001,
  bit1 = 0x00000002,
  bit2 = 0x00000004,
  bit3 = 0x00000008,
  bit4 = 0x00000010,
  bit5 = 0x00000020,
};

enum off_masks
{
  nbit0 = ~bit0,
  nbit2 = ~bit1,
  nbit3 = ~bit3,
  nbit4 = ~bit4,
  nbit5 = ~bit5,
};

// TODO

typedef struct blockmap
{
  uint32_t *blocks;
  size_t size;
} blockmap;

blockmap map;
#define WORDSIZE 32
#define MASK 0x1F

void set_bit(uint32_t *pi, int n)
{
  *pi |= 1 << (n & MASK);
}

int check_bit(uint32_t i, int n)
{
  return i & 1 << (n & MASK);
}

void clear_bit(uint32_t i, int n)
{
  i &= ~(1 << (n & MASK));
}

// legacy function
int count_free_bits(uint32_t i, int n)
{
  int m = 0;
  for (m = 0; m < WORDSIZE - n; m++)
  {
    if (check_bit(i, n + m))
    {
      break;
    }
  }
  return m;
}

// legacy function
int find_free_bit(uint32_t i)
{
  for (int n = 0; n < WORDSIZE; n++)
  {
    if (check_bit(i, n))
    {
      return n;
    }
  }
  return -1;
}

// debug print dump of map
void print_map(int n)
{
  // printf("printmap: %ld\n", map.size);
  for (int i = 0; i < map.size; i += 8)
  {
    int foo = (i > map.size - 8) ? map.size % 8 : 8;
    for (int x = 0; x < foo; x++)
    {
      printf("[ %x ]", map.blocks[i * 8 + x]);
    }
    printf("\n");
  }
  printf("--\n");
}

// alloc for bitmap
int init_free_space(int block_count, int block_size)
{
  if (block_count < 1)
  {
    perror("Invalid block count\n");
  }
  map.size = (block_count - 1) / WORDSIZE + 1;
  map.blocks = (uint32_t *)calloc(map.size + 1, sizeof(uint32_t));
  if (map.blocks == NULL)
  {
    perror("init_bit_map: map.blocks calloc failed\n");
    return -1;
  }
  // mask inital bits for vcb and bitmap
  map.blocks[0] = bit0 | bit1 | bit2 | bit3 | bit4 | bit5;
  LBAwrite(map.blocks, 5, 1);
  // return block at start of free space
  return 2;
}

// a cool idea that didn't pan out
int get_count(int index)
{
  // I bet you recognize this one
  unsigned int word = index; // 32-bit word input
  unsigned int count;        // count will be the number of zero bits on the right,
  // NOTE: if 0 == word, then count = 31.
  if (word & 0x1)
  {
    // handle odd word
    count = 0;
  }
  else
  {
    count = 1;
    if ((word & 0xffff) == 0)
    {
      word >>= 16;
      count += 16;
    }
    if ((word & 0xff) == 0)
    {
      word >>= 8;
      count += 8;
    }
    if ((word & 0xf) == 0)
    {
      word >>= 4;
      count += 4;
    }
    if ((word & 0x3) == 0)
    {
      word >>= 2;
      count += 2;
    }
    count -= word & 0x1;
  }
  return count;
}

// check a range for a given bit by state
// if found, return its offset
int next_block(int start, int end, int match)
{
  int begin = 0;
  int found = 0;
  for (begin = start; begin <= end; begin++)
  {
    int c = check_bit(map.blocks[begin / WORDSIZE], begin % WORDSIZE);
    if ((match && c) || (!match && !c) || begin == end)
    {
      found = 1;
      break;
    }
  }
  return found ? begin : -1;
}

// set bits corresponding to passed extent
void mark_extent(int start, int length)
{
  for (int i = 0; i < length; i++)
  {
    set_bit(map.blocks + (start + i) / WORDSIZE, (start % WORDSIZE) + i);
  }
}

// find satisfactory extents of free space
int get_extent(int start, int req, int min_size, extent *pextent)
{
  int begin = start;
  while (1)
  {
    int foo = (begin + min_size < map.size * WORDSIZE) ? begin + min_size : map.size * WORDSIZE;
    begin = next_block(start, foo, 0);
    if (begin == -1)
    {
      perror("begin error");
      return -1; // found nothing
    }
    foo = (begin + min_size < map.size * WORDSIZE) ? begin + min_size : map.size * WORDSIZE;
    int end = next_block(begin + 1, foo, 1);
    if (end == -1)
    {
      perror("end error");
      return -1; // no free disk block
    }
    if (end - begin < min_size)
    {
      begin += end;
      continue;
    }
    pextent->count = end - begin;
    pextent->start = begin;
    return 0;
  }
}

// find empty space as extents
// set extents' bits
// rewrite the free space map
// return the extents
// TODO bad contract with dirent extents system
// Need to return 3 extents not arbitrary
extent *allocate_blocks(int blocks_required, int min_extent_size)
{
  int max_extents = blocks_required / min_extent_size + !!(blocks_required % min_extent_size);
  extent *rc = (extent *)calloc(max_extents, sizeof(extent));
  int start = 6; // first six blocks are vcb and map
  if (rc == NULL)
  {
    perror("rc calloc failed\n");
    return NULL;
  }
  for (int i = 0; i < max_extents; i++)
  {
    int success = get_extent(start, blocks_required, min_extent_size, rc + i);
    if (success == -1)
    {
      perror("Insufficient disk space!");
      free(rc);
      return NULL;
    }
    blocks_required -= rc[i].count;
    start = rc[i].start + rc[i].count;
  }
  for (int i = 0; i < max_extents; i++)
  {
    mark_extent(rc[i].start, rc[i].count);
  }
  LBAwrite(map.blocks, 5, 1);

  return rc;
}

// local debug testing
//  int main(int argv, char *argc[])
//  {
//    // init_free_space(19531, 512);
//    test_bit_functions();
//    return 0;
//  }
