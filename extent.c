#include <stdio.h>



#include "extent.h"

void extent_print(uint (*extent)[2]) {
    // TODO:
}

unsigned int extent_get_block_num(uint (*extent)[2], uint index) {
    // TODO:
	return 0;
}

unsigned int extent_append(uint (*extent)[2], uint block_number, uint count) {
    // TODO:
	return 0;
}

unsigned int extent_remove_blocks(uint (*extent)[2], uint block_number, uint count) {
    // TODO:
	return 0;
}


/////////// ------- malloc here???????
void extent_init(uint (*extent)[2]) {
	
    for (uint i = 0; i < EXTENT_SIZE; i++) {
        for (uint j = 0; j < 2; j++) {
            extent[i][j]= 0;
        }
    }
}

unsigned int extent_get_total_blocks(uint (*extent)[2]) {
    // TODO:
	
	return 0;
}





