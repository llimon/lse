#!/bin/sh
gcc -x c -std=gnu99 -DTEST_GETPROGNAME_COMPAT getprogname_compat.h -o getprogname
