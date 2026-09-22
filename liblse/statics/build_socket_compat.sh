#!/bin/sh
gcc -x c -std=gnu99 -D_TEST_SOCKET_COMPAT_CUSTOM -lposix4 -lsocket -lnsl socket_compat.h -o socket_compat
