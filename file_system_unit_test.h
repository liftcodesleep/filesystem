
#ifndef FILE_SYSTEM_UNIT_TESTS
#define FILE_SYSTEM_UNIT_TESTS

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "fsLow.h"
#include "mfs.h"
#include "vcb.h"
#include "dirEntry.h"
#include "b_bitmap.h"
#include "extent.h"

void test(int (*func)(), char *name);
void file_system_unit_tests();

#endif