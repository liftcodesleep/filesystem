#ifndef EXTENT_H
#define EXTENT_H

#define EXTENT_SIZE 3

typedef unsigned int uint;

/* 
 * For debugging purposes, prints the contents of the extent map.
 */
void extent_print(uint (*extent)[2]);

/*
 * Returns:
 *   - The block number at the specified index.
 */
unsigned int extent_get_block_num(uint (*extent)[2], uint index);

/*
 * Adds more blocks to the extent.
 * Returns:
 *   - The number of blocks successfully added.
 */
unsigned int extent_append(uint (*extent)[2], uint block_number, uint count);

/*
 * Removes blocks from the extent.
 * Returns:
 *   - The number of blocks successfully removed.
 */
unsigned int extent_remove_blocks(uint (*extent)[2], uint block_number, uint count);

/*
 * Initializes the extent to its initial state.
 */
void extent_init(uint (*extent)[2]);


/*
 * Returns:
 *   - The total number of blocks in the extent.
 */
unsigned int extent_get_total_blocks(uint (*extent)[2]);



#endif /* EXTENT_H */