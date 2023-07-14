#include "b_io.h"
typedef struct jextent
{
  int start;
  int count;
} jextent, *pextent;

// make space return first free block num
int init_free_space(int block_count, int block_size);

// get space
int load_free_space(int block_count, int block_size);

// use this to size things to fit our extents
jextent *allocate_blocks(int blocks_required, int min_per_extent);

// frees an jextent's worth
void release_blocks(int start, int count);
