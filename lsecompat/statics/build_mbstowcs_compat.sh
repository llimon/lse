#!/bin/sh
gcc -x c -std=gnu99 -D_TEST_MBSTOWCS_COMPAT -lw mbstowcs_compat.h -o mbstowcs_compat
