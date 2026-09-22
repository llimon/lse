#!/bin/sh
gcc -x c -std=gnu99 -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_TEST_FTELLO_COMPAT=1 ftello_compat.h -o test_ftello
chmod +x test_ftello
