#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> // for malloc
#include "b_bitmap.h"

// how big is a dirent? how many blocks for root?
enum on_masks
{
  bit0 = 0x000000001,
  bit1 = 0x000000002,
  bit2 = 0x000000004,
  bit3 = 0x000000008,
  bit4 = 0x000000010,
  bit5 = 0x000000020,
};

enum off_masks
{
  nbit0 = ~bit0,
  nbit2 = ~bit1,
  nbit3 = ~bit3,
  nbit4 = ~bit4,
  nbit5 = ~bit5,
};

typedef struct blockmap
{
  uint32_t *blocks;
  size_t size;
} blockmap;

blockmap map;
#define WORDSIZE 32
#define MASK 0x1F

void set_bit(int index)
{
  map.blocks[index / WORDSIZE] |= 1 << (index & MASK);
}

int init_free_space(int block_count, int block_size)
{
  map.size = block_count;
  printf("map.size = %ld\n", map.size);
  map.blocks = (uint32_t *)calloc(map.size + 1, sizeof(uint32_t));
  if (map.blocks == NULL)
  {
    perror("init_bit_map: map.blocks calloc failed\n");
    return -1;
  }
  map.blocks[0] = bit0 | bit1 | bit2 | bit3 | bit4 | bit5;
  return 6;
}

uint32_t check_bit(uint32_t index)
{
  if (map.blocks[index / WORDSIZE] & 1 << (index & MASK))
  {
    return 1;
  }
  return 0;
}

void clear_bit(int index)
{
  map.blocks[index / WORDSIZE] &= ~(1 << (index & MASK));
}

int get_count(int index)
{
  {
    unsigned int v = index; // 32-bit word input to count zero bits on right
    unsigned int c;         // c will be the number of zero bits on the right,
                            // so if v is 1101000 (base 2), then c will be 3
    // NOTE: if 0 == v, then c = 31.
    if (v & 0x1)
    {
      // special case for odd v (assumed to happen half of the time)
      c = 0;
    }
    else
    {
      c = 1;
      if ((v & 0xffff) == 0)
      {
        v >>= 16;
        c += 16;
      }
      if ((v & 0xff) == 0)
      {
        v >>= 8;
        c += 8;
      }
      if ((v & 0xf) == 0)
      {
        v >>= 4;
        c += 4;
      }
      if ((v & 0x3) == 0)
      {
        v >>= 2;
        c += 2;
      }
      c -= v & 0x1;
    }
  }
}

extent *allocate_blocks(int blocks_required, int min_extent_size)
{
  extent *free_space = (extent *)malloc(blocks_required * sizeof(extent));
  int start = 0;
  int count = 0;
  int num_extents = 0;
  // for each word
  for (int index = 0; index < map.size / WORDSIZE; index++)
  {
    // if the word is not full
    if (map.blocks[index / WORDSIZE] != 2 ^ WORDSIZE - 1)
    {
      // check for empty bit
      for (int bit = 0; bit < WORDSIZE; bit++)
      {
        if (check_bit(map.blocks[index * WORDSIZE + bit]) == 0)
        {
          start = index * WORDSIZE + bit;
          count = 1;
          while (check_bit(map.blocks[index * WORDSIZE + bit]) == 0 && count < blocks_required)
          {
            count++;
            bit++;
          }
          if (count >= min_extent_size)
          {
            free_space[num_extents].count = count;
            free_space[num_extents].start = start;
            blocks_required -= count;
            num_extents++;
            if (blocks_required == 0)
            {
              return free_space;
            }
          }
        }
      }
    }
  }
  // for int in map.blocks
  // if free note and count until not free
  // return note and count
}

void test_bit_functions()
{
  for (int index = 0; index < 33; index = index + 1)
  {
    check_bit(index);
    printf("test_bit_functions init:  index: %d bit is 2^%d checkbit reads %d\n", index, index, check_bit(index));
    set_bit(index);
    printf("test_bit_functions set:   index: %d set bit %d\n", index, index);
    check_bit(index);
    printf("test_bit_functions check: index: %d bit is 2^%d checkbit reads %d\n", index, index, check_bit(index));
    // clear_bit(index);
    // printf("test_bit_functions clear: index: %d bit is: %u\n", index, map.blocks[index / WORDSIZE]);
    // check_bit(index);
    // printf("test_bit_functions check: index: %d bit is: %u\n", index, map.blocks[index / WORDSIZE]);
  }
}

int main(int argv, char *argc[])
{
  init_free_space(19531, 512);
  test_bit_functions();
  return 0;
}