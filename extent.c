
#include "extent.h"



void extent_print(Extent *extent) {
    
	printf("Location    Count\n");
	for(int i = 0;i < EXTENT_SIZE; i++ ){
		printf("%05d        %05d\n", extent[i].start_loc, extent[i].amount_after_start);
	}
	
}

int extent_get_block_num(Extent *extent, uint index) {
    
	int block_index = 0;
	for(int i = 0;i < EXTENT_SIZE; i++ ){
		
		if(extent[i].amount_after_start + block_index > index){
			return extent[i].start_loc +  index - block_index;
		}
		block_index += extent[i].amount_after_start;
	}
	
	return -1;
}

unsigned int extent_append(Extent *extent, uint block_number, uint count) {
	
	uint current_index;
	for(current_index = 0;current_index < EXTENT_SIZE; current_index++ ){
		
		
		if(extent[current_index].amount_after_start == 0){
			
			if(current_index > 0 && extent[current_index-1].amount_after_start + extent[current_index-1].start_loc == block_number){
				extent[current_index-1].amount_after_start += count;
				return count;
			}
			
			extent[current_index].start_loc = block_number;
			extent[current_index].amount_after_start = count;
			return count;
		}

	}
	
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
    int block_total = 0;
	for(int i = 0;i < EXTENT_SIZE; i++ ){
		block_total += extent[i].amount_after_start;
	}
	
	return block_total;
}

void extent_free(Extent *extent){
	
	free(extent);
}






int _extent_init_second(Extent *extent){
	
	// TODO:
	return 0;
}





