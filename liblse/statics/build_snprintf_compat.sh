#!/bin/sh
gcc -x c -std=gnu99 -llsecompat -Wl,-rpath,/usr/local/lse/lib -D_TEST_SNPRINTF_COMPAT snprintf_compat.h -o snprintf_compat
