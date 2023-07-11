 
#include "extent.c"
 
 ///////////// UNIT TESTS //////////////////////////

void test_extent_init() {
    uint extent[EXTENT_SIZE][2];
    extent_init(extent);

    printf("extent_init test:\n");
    printf("Verifying extent initialization...\n");

    // Check if the extent is initialized to 0
    int i, j;
    int failed = 0;
    for (i = 0; i < EXTENT_SIZE; i++) {
        for (j = 0; j < 2; j++) {
            if (extent[i][j] != 0) {
                failed = 1;
                break;
            }
        }
    }

    // Print the result
    if (failed) {
        printf("Failed\n");
    } else {
        printf("Passed\n");
    }
}

void test_extent_get_block_num() {
    uint extent[EXTENT_SIZE][2];
    extent_init(extent);

    // Append 5 blocks starting from block number 10
    extent_append(extent, 10, 5);

    // Test extent_get_block_num for different indices
    unsigned int blockNum1 = extent_get_block_num(extent, 0);
    unsigned int blockNum2 = extent_get_block_num(extent, 2);
    unsigned int blockNum3 = extent_get_block_num(extent, 4);

    printf("extent_get_block_num test:\n");
    printf("Block number at index 0: %u\n", blockNum1);
    printf("Block number at index 2: %u\n", blockNum2);
    printf("Block number at index 4: %u\n", blockNum3);

    // Verify the results
    if (blockNum1 == 10 && blockNum2 == 12 && blockNum3 == 14) {
        printf("Passed\n");
    } else {
        printf("Failed\n");
    }
}

void test_extent_append() {
    uint extent[EXTENT_SIZE][2];
    extent_init(extent);

    // Append 5 blocks starting from block number 10
    unsigned int count = extent_append(extent, 10, 5);

    printf("extent_append test:\n");
    printf("Number of blocks appended: %u\n", count);

    // Verify the result
    if (count == 5) {
        printf("Passed\n");
    } else {
        printf("Failed\n");
    }
}

void test_extent_remove_blocks() {
    uint extent[EXTENT_SIZE][2];
    extent_init(extent);

    // Append 5 blocks starting from block number 10
    extent_append(extent, 10, 5);

    // Remove 3 blocks starting from block number 12
    unsigned int count = extent_remove_blocks(extent, 12, 3);

    printf("extent_remove_blocks test:\n");
    printf("Number of blocks removed: %u\n", count);

    // Verify the result
    if (count == 3) {
        printf("Passed\n");
    } else {
        printf("Failed\n");
    }
}



void test_extent_get_total_blocks() {
    uint extent[EXTENT_SIZE][2];
    extent_init(extent);

    // Append 5 blocks starting from block number 10
    extent_append(extent, 10, 5);

    // Get the total number of blocks in the extent
    unsigned int totalBlocks = extent_get_total_blocks(extent);

    printf("extent_get_total_blocks test:\n");
    printf("Total number of blocks in extent: %u\n", totalBlocks);

    // Verify the result
    if (totalBlocks == 5) {
        printf("Passed\n");
    } else {
        printf("Failed\n");
    }
}

int main() {
    printf("Running extent tests...\n\n");

	test_extent_init();
    printf("\n");
	
    test_extent_get_block_num();
    printf("\n");

    test_extent_append();
    printf("\n");

    test_extent_remove_blocks();
    printf("\n");

    test_extent_get_total_blocks();
    printf("\n");

    return 0;
}