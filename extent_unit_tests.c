 
#include "extent.c"
 
 ///////////// UNIT TESTS //////////////////////////

void test_extent_init() {
    Extent extent[EXTENT_SIZE];
    extent_init(extent);
	extent_print(extent);
    char question[] = "extent_init test: ";
    //char question[] = ( "Verifying extent initialization...\n");

    // Check if each extent element is initialized correctly
    int i;
    int failed = 0;
    for (i = 0; i < EXTENT_SIZE; i++) {
        if (extent[i].start_loc != 0 || extent[i].amount_after_start != 0) {
            failed = 1;
            break;
        }
    }

    // Print the result
    if (failed) {
        printf( "%-32s Failed\n", question);
    } else {
        printf( "%-32s Passed\n", question);
    }
}

void test_extent_get_block_num() {
    Extent extent[EXTENT_SIZE];
    extent_init(extent);

    // Append 5 blocks starting from block number 10
    extent_append(extent, 10, 5);

    // Test extent_get_block_num for different indices
    unsigned int blockNum1 = extent_get_block_num(extent, 0);
    unsigned int blockNum2 = extent_get_block_num(extent, 2);
    unsigned int blockNum3 = extent_get_block_num(extent, 4);

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
}

void test_extent_append() {
    Extent extent[EXTENT_SIZE];
    extent_init(extent);

    // Append 5 blocks starting from block number 10
    unsigned int count = extent_append(extent, 10, 5);

    char question[] = "extent_append test: ";
    // char question[] = ( "Number of blocks appended: %u\n", count);

    // Verify the result
    if (count == 5) {
        printf( "%-32s Passed\n", question);
    } else {
        printf( "%-32s Failed\n", question);
    }
}

void test_extent_remove_blocks() {
    Extent extent[EXTENT_SIZE];
    extent_init(extent);

    // Append 5 blocks starting from block number 10
    extent_append(extent, 10, 5);

    // Remove 3 blocks starting from block number 12
    unsigned int count = extent_remove_blocks(extent, 12, 3);

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
    Extent extent[EXTENT_SIZE];
    extent_init(extent);

    // Append 5 blocks starting from block number 10
    extent_append(extent, 10, 5);

    // Get the total number of blocks in the extent
    unsigned int totalBlocks = extent_get_total_blocks(extent);

    char question[] = "extent_get_total_blocks test: ";
    //char question[] = ( "Total number of blocks in extent: %u\n", totalBlocks);

    // Verify the result
    if (totalBlocks == 5) {
        printf( "%-32s Passed\n", question);
    } else {
        printf( "%-32s Failed\n", question);
    }
}

int main() {
    printf("Running extent tests...\n\n");

    test_extent_init();

    test_extent_get_block_num();

    test_extent_append();

    test_extent_remove_blocks();

    test_extent_get_total_blocks();


    return 0;
}