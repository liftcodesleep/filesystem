
#include <stdlib.h>
#include "extent.c"
 
///////////// UNIT TESTS //////////////////////////



jextent* make_test_extent() {
    jextent* jextent = malloc(sizeof(jextent) * 3);

    extent_init(jextent);

    return jextent;
}

void test_extent_init() {
    jextent* jextent = make_test_extent();

    char question[] = "extent_init test: ";

    // Check if each jextent element is initialized correctly
    int i;
    int failed = 0;
    for (i = 0; i < EXTENT_SIZE; i++) {
        if (jextent[i].start != 0 || jextent[i].count != 0) {
            failed = 1;
            break;
        }
    }

    // Print the result
    if (failed) {
        printf("%-32s Failed\n", question);
    } else {
        printf("%-32s Passed\n", question);
    }
	
	free(jextent);
}

void test_extent_get_block_num() {
    jextent* jextent = make_test_extent();
    

    // Append 5 blocks starting from block number 10
    extent_append(jextent, 10, 5);

    // Test extent_get_block_num for different indices
    unsigned int blockNum1 = extent_get_block_num(jextent, 0);
    unsigned int blockNum2 = extent_get_block_num(jextent, 2);
    unsigned int blockNum3 = extent_get_block_num(jextent, 4);

    char question[] = "extent_get_block_num test: ";
    //char question[] = ( "Block number at index 0: %u\n", blockNum1);
    //char question[] = ( "Block number at index 2: %u\n", blockNum2);
    //char question[] = ( "Block number at index 4: %u\n", blockNum3);

    // Verify the results
    if (blockNum1 == 10 && blockNum2 == 12 && blockNum3 == 14) {
        printf( "%-32s Passed\n", question);
    } else {
        printf( "%-32s Failed\n", question);
    }
	
	free(jextent);
}

void test_extent_append() {
    jextent* jextent = make_test_extent();

    // Append 5 blocks starting from block number 10
    unsigned int count1 = extent_append(jextent, 10, 5);
	unsigned int count2 = extent_append(jextent, 15, 6);
	unsigned int count3 = extent_append(jextent, 30, 1);
    char question[] = "extent_append test: ";
    // char question[] = ( "Number of blocks appended: %u\n", count);

    // Verify the result
    if (count1 == 5 && count2 == 6 && count3 == 1) {
        printf( "%-32s Passed\n", question);
    } else {
        printf( "%-32s Failed\n", question);
    }
	
	free(jextent);
}

void test_extent_remove_blocks() {
    jextent jextent[EXTENT_SIZE];
    extent_init(jextent);

    // Append 5 blocks starting from block number 10
    extent_append(jextent, 10, 5);

    // Remove 3 blocks starting from block number 12
    unsigned int count = extent_remove_blocks(jextent, 12, 3);

    char question[] = "extent_remove_blocks test: ";
    //char question[] = ( "Number of blocks removed: %u\n", count);

    // Verify the result
    if (count == 3) {
        printf( "%-32s Passed\n", question);
    } else {
        printf( "%-32s Failed\n", question);
    }
}

void test_extent_get_total_blocks() {
    jextent jextent[EXTENT_SIZE];
    extent_init(jextent);

    // Append 5 blocks starting from block number 10
    extent_append(jextent, 10, 5);

    // Get the total number of blocks in the jextent
    unsigned int totalBlocks = extent_get_total_blocks(jextent);

    char question[] = "extent_get_total_blocks test: ";
    //char question[] = ( "Total number of blocks in jextent: %u\n", totalBlocks);

    // Verify the result
    if (totalBlocks == 5) {
        printf( "%-32s Passed\n", question);
    } else {
        printf( "%-32s Failed\n", question);
    }
}

int main() {
    printf("Running jextent tests...\n\n");

    test_extent_init();

    test_extent_get_block_num();

    test_extent_append();

    //test_extent_remove_blocks();

    test_extent_get_total_blocks();


    return 0;
}