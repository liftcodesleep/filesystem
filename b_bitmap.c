#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> // for malloc
#include "b_bitmap.h"

// how big is a dirent? how many blocks for root?
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
  // printf("set_bit: pi: %d n: %d mask: %d shift mask: %d\n", *pi, n, n & MASK, 1 << (n & MASK));
  *pi |= 1 << (n & MASK);
  // printf("set_bit: pi: %d n: %d\n", *pi, n);
}

int check_bit(uint32_t i, int n)
{
  return i & 1 << (n & MASK);
}

void clear_bit(uint32_t i, int n)
{
  i &= ~(1 << (n & MASK));
}

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

int init_free_space(int block_count, int block_size)
{
  // FIXME: small enough block_count gives map size 0
  map.size = block_count / WORDSIZE;
  map.blocks = (uint32_t *)calloc(map.size + 1, sizeof(uint32_t));
  if (map.blocks == NULL)
  {
    perror("init_bit_map: map.blocks calloc failed\n");
    return -1;
  }
  // print_map(0);
  map.blocks[0] = bit0 | bit1 | bit2 | bit3 | bit4 | bit5;
  // LBAwrite(map.blocks, 5, 1);
  // print_map(0);
  return 1;
}

int get_count(int index)
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
  return c;
}

int next_block(int start, int end, int match)
{
  int begin = 0;
  int found = 0;
  // printf("next_block: start: %d end: %d match: %d\n", start, end, match);
  for (begin = start; begin <= end; begin++)
  {
    int c = check_bit(map.blocks[begin / WORDSIZE], begin % WORDSIZE);
    // printf("next_block match: %d c: %d\n", match, c);
    if ((match && c) || (!match && !c) || begin == end)
    {
      found = 1;
      break;
    }
  }
  // printf("next_block: returning: %d\n", found ? begin : -1);
  return found ? begin : -1;
}

void mark_extent(int start, int length)
{
  // printf("mark_extent: start: %d length %d\n", start, length);
  for (int i = 0; i < length; i++)
  {
    set_bit(map.blocks + start / WORDSIZE, (start + i) % WORDSIZE);
  }
}

int get_extent(int start, int req, int min_size, jextent *pextent)
{
  // printf("get_extent: start: %d req: %d min_size: %d\n", start, req, min_size);
  int begin = start;
  while (1)
  {
    int foo = (begin + min_size < map.size * WORDSIZE) ? begin + min_size : map.size * WORDSIZE;
    // printf("get_extent: begin: %d min_size: %d map.size %ld foo: %d\n", begin, min_size, map.size, foo);
    begin = next_block(start, foo, 0);
    if (begin == -1)
    {
      return -1; // found nothing
    }
    foo = (begin + min_size < map.size * WORDSIZE) ? begin + min_size : map.size * WORDSIZE;
    // printf("get_extent: begin: %d min_size: %d map.size %ld foo: %d\n", begin, min_size, map.size, foo);
    int end = next_block(begin + 1, foo, 1);
    if (end == -1)
    {
      return -1; // no free disk block
    }
    if (end - begin < min_size)
    {
      begin += end;
      continue;
    }
    // printf("end: %d begin: %d\n", end, begin);
    pextent->count = end - begin;
    pextent->start = begin;
    // printf("get_ext: ext.count: %d ext.start %d\n", pextent->count, pextent->start);
    return 0;
  }
}

jextent *allocate_blocks(int blocks_required, int min_extent_size)
{
  int max_extents = blocks_required / min_extent_size + !!(blocks_required % min_extent_size);
  jextent *rc = (jextent *)calloc(max_extents, sizeof(jextent));
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
    start += rc[i].count;
  }
  for (int i = 0; i < max_extents; i++)
  {
    printf("allocate_blocks: i: %d start: %d count: %d\n", i, rc[i].start, rc[i].count);
    mark_extent(rc[i].start, rc[i].count);
  }
  return rc;
}

void test_bit_functions()
{
  for (int index = 0; index < 33; index = index + 1)
  {
    // printf("test_bit_functions init:  index: %d bit is 2^%d checkbit reads %d\n", index, index, check_bit(index));
    // set_bit(index);
    // printf("test_bit_functions set:   index: %d set bit %d\n", index, index);
    // printf("test_bit_functions check: index: %d bit is 2^%d checkbit reads %d\n", index, index, check_bit(index));
    // clear_bit(index);
    // printf("test_bit_functions check: index: %d bit is 2^%d checkbit reads %d\n", index, index, check_bit(index));
  }
  print_map(0);
  allocate_blocks(1, 1);
  print_map(0);
  printf("----------------------------\n");
  print_map(0);
  allocate_blocks(2, 2);
  print_map(0);
  printf("----------------------------\n");
  print_map(0);
  allocate_blocks(50, 10);
  print_map(0);
}

int main(int argv, char *argc[])
{
  // init_free_space(19531, 512);
  init_free_space(640, 512);
  test_bit_functions();
  return 0;
}