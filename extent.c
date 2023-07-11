#include <stdio.h>

#include "extent.h"



void extent_print(Extent *extent) {
    
	printf("Location    Count\n");
	for(int i = 0;i < EXTENT_SIZE; i++ ){
		
		printf("%05d        %05d\n", extent->start_loc, extent->amount_after_start);
		
	}
	
	
	
}

unsigned int extent_get_block_num(Extent *extent, uint index) {
    // TODO:
	return 0;
}

unsigned int extent_append(Extent *extent, uint block_number, uint count) {
    // TODO:
	return 0;
}

unsigned int extent_remove_blocks(Extent *extent, uint block_number, uint count) {
    // TODO:
	return 0;
}



void extent_init(Extent *extent) {
	
    for (uint i = 0; i < EXTENT_SIZE; i++) {
        extent[i].start_loc = 0;
		extent[i].amount_after_start = 0;
    }
}

unsigned int extent_get_total_blocks(Extent *extent) {
    // TODO:
	
	return 0;
}





