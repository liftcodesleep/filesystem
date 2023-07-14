#ifndef EXTENT_H
#define EXTENT_H

#include <stdio.h>
#include <stdlib.h>
#include "b_bitmap.h"
#include "fsLow.h"

#define EXTENT_SIZE 3

#define SENTINAL_VALUE 0xffffffff

typedef unsigned int uint;

////// Change jextent to an array of struct 



/*
 * Initializes the jextent to its initial state.
 */
void extent_init(jextent *jextent);

void extent_free(jextent *jextent);

/*
 * Returns:
 *   - The block number at the specified index.
 * Error:
 * Reutrns -1
 */
int extent_get_block_num(jextent *jextent, uint index);

/*
 * Adds more blocks to the jextent. Also merges the jextent
 * Returns:
 *   - The number of blocks successfully added.
 */
unsigned int extent_append(jextent *jextent, uint block_number, uint count);

/*
 * Removes blocks from the jextent.
 * Returns:
 *   - The number of blocks successfully removed.
 */
unsigned int extent_remove_blocks(jextent *jextent, uint block_number, uint count);

/*
 * Returns:
 *   - The total number of blocks in the jextent.
 */
unsigned int extent_get_total_blocks(jextent *jextent);


/*
* merges the second jextent into the first jextent
* Returns:
*   - The amount of blocks merged 
*
*/
int extent_merge(jextent *first_extent, jextent *second_extent);


/* 
 * For debugging purposes, prints the contents of the jextent map.
 */
void extent_print(jextent *jextent);





#endif /* EXTENT_H */