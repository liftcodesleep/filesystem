#ifndef EXTENT_H
#define EXTENT_H

#define EXTENT_SIZE 3
#define SENTINAL_VALUE 0xffff

typedef unsigned int uint;

////// Change extent to an array of struct 

typedef struct Extent{
	
	uint start_loc;
	uint amount_after_start;
	
}Extent;


/*
 * Initializes the extent to its initial state.
 */
void extent_init(Extent *extent);



/*
 * Returns:
 *   - The block number at the specified index.
 */
unsigned int extent_get_block_num(Extent *extent, uint index);

/*
 * Adds more blocks to the extent. Also merges the extent
 * Returns:
 *   - The number of blocks successfully added.
 */
unsigned int extent_append(Extent *extent, uint block_number, uint count);

/*
 * Removes blocks from the extent.
 * Returns:
 *   - The number of blocks successfully removed.
 */
unsigned int extent_remove_blocks(Extent *extent, uint block_number, uint count);

/*
 * Returns:
 *   - The total number of blocks in the extent.
 */
unsigned int extent_get_total_blocks(Extent *extent);

/* 
 * For debugging purposes, prints the contents of the extent map.
 */
void extent_print(Extent *extent);


#endif /* EXTENT_H */