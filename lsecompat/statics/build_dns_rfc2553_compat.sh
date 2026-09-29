#!/bin/sh
gcc -x c -std=gnu99 -D_TEST_DNS_RFC2553_COMPAT -lsocket -lnsl dns_rfc2553_compat.h -o dns_rfc2553_compat
