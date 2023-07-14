
#include "extent.h"


void extent_print(jextent *jextent) {
    
	printf("Location    Count\n");
	for(int i = 0;i < EXTENT_SIZE; i++ ){
		printf("%05d        %05d\n", jextent[i].start, jextent[i].count);
	}
	
}

int extent_get_block_num(jextent *jextent, uint index) {
    
	int block_index = 0;
	for(int i = 0;i < EXTENT_SIZE; i++ ){
		
		if(jextent[i].count + block_index > index){
			return jextent[i].start +  index - block_index;
		}
		block_index += jextent[i].count;
	}
	
	return -1;
}

unsigned int extent_append(jextent *jextent, uint block_number, uint count) {
	
	uint current_index;
	for(current_index = 0;current_index < EXTENT_SIZE; current_index++ ){
		
		
		if(jextent[current_index].count == 0){
			
			if(current_index > 0 && jextent[current_index-1].count + jextent[current_index-1].start == block_number){
				jextent[current_index-1].count += count;
				return count;
			}
			
			jextent[current_index].start = block_number;
			jextent[current_index].count = count;
			return count;
		}

	}
	
    return 0;
}

unsigned int extent_remove_blocks(jextent *jextent, uint block_number, uint count) {
    // TODO:
	return 0;
}


void extent_init(jextent *jextent) {
	
    for (uint i = 0; i < EXTENT_SIZE; i++) {
        jextent[i].start = 0;
		jextent[i].count = 0;
    }
}

unsigned int extent_get_total_blocks(jextent *jextent) {
    int block_total = 0;
	for(int i = 0;i < EXTENT_SIZE; i++ ){
		block_total += jextent[i].count;
	}
	
	return block_total;
}

void extent_free(jextent *jextent){
	
	free(jextent);
}


///*
//* Makes a new 2nd jextent with the first value as the jextent passed
//* Returns the block where the table is saved
//*/
//int make_2nd_extent_table(jextent* extent){
//
//	jextent* p_loc = allocate_blocks(1, 1);
//
//	LBAwrite(extent, 1, p_loc->start);
//
//
//	return p_loc->start;
//}
//
///*
//* Return a pointer to an jextent at the block loation given
//*/
//jextent* load_2nd_extent_table(uint block_number){
//
//	jextent* table = malloc( sizeof(jextent)*64 );
//
//	LBAread(table, 1, block_number);
//
//	return table;
//
//
//}
//
//
///*
//* Makes a new 3nd jextent with the first value as the location passed
//* Returns the block where the table is saved
//*/
//int make_3rd_extent_table(uint location){
//
//	jextent* p_loc = allocate_blocks(1, 1);
//
//	uint* location_buffer = &location;
//
//	LBAwrite(location_buffer, 1, p_loc->start);
//
//
//	return p_loc->start;
//}
//
///*
//* Return a pointer to an jextent at the block loation given
//*/
//jextent* load_3rd_extent_table(uint block_number){
//
//	jextent* table = malloc( sizeof(block_number)*128 );
//
//	LBAread(table, 1, block_number);
//
//	return table;
//
//}







