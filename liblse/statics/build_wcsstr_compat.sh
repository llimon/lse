#!/bin/sh
gcc -x c -std=gnu99 -D_TEST_WCSSTR_COMPAT -lw wcsstr_compat.h -o wcsstr_compat
