#!/bin/sh
gcc -x c -std=gnu99 -D_TEST_USLEEP_COMPAT usleep_compat.h -o usleep_compat
