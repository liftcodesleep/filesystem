
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


/*
* Makes a new 2nd extent with the first value as the extent passed
* Returns the block where the table is saved
*/
int make_2nd_extent_table(Extent* extent){

	Extent* p_loc = allocate_blocks(1, 1);

	LBAwrite(extent, 1, p_loc->start_loc);


	return p_loc->start_loc;
}

/*
* Return a pointer to an extent at the block loation given
*/
Extent* load_2nd_extent_table(uint block_number){

	Extent* table = malloc( sizeof(Extent)*64 );

	LBAread(table, 1, block_number);

	return table;


}


/*
* Makes a new 3nd extent with the first value as the location passed
* Returns the block where the table is saved
*/
int make_3rd_extent_table(uint location){

	Extent* p_loc = allocate_blocks(1, 1);

	uint* location_buffer = &location;

	LBAwrite(location_buffer, 1, p_loc->start_loc);


	return p_loc->start_loc;
}

/*
* Return a pointer to an extent at the block loation given
*/
uint* load_3rd_extent_table(uint block_number){

	Extent* table = malloc( sizeof(block_number)*128 );

	LBAread(table, 1, block_number);

	return table;


}







