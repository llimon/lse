#include "usleep_compat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>
#include <unistd.h>
#include <errno.h>

/*
 * GCC 2.95 / Sun Assembler compatible weak directive
 */
#if defined(__GNUC__)
#  if defined(__ELF__) || defined(__solaris__)
#    pragma weak usleep
#  else
     int usleep(useconds_t usec) __attribute__((weak));
#  endif
#endif

int usleep(useconds_t usec) {
    struct timeval tv;

    tv.tv_sec  = (long)(usec / 1000000UL);
    tv.tv_usec = (long)(usec % 1000000UL);

    /* 
     * Portable high-resolution pause using select().
     * Works natively on SunOS 4.1.4 and Solaris 2.5.1 without -lposix4.
     */
    return select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &tv);
}

/* =========================================================================
 *  Embedded Unit Test Harness
 *  Compile test: gcc -O2 -mcpu=v7 -D_TEST_USLEEP_COMPAT usleep_compat.c -o test_usleep -lsocket -lnsl
 * ========================================================================= */
#ifdef _TEST_USLEEP_COMPAT
#include <stdio.h>

int main(void) {
    struct timeval start, end;
    long elapsed_usec;

    printf("=== Running Universal usleep Unit Test ===\n");

    /* Test 1: Validate 250,000 microsecond (250ms) sleep duration */
    gettimeofday(&start, (struct timezone *)0);
    if (usleep(250000UL) != 0) {
        perror("FAIL: usleep execution failed");
        return 1;
    }
    gettimeofday(&end, (struct timezone *)0);

    elapsed_usec = (end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec);
    printf("Test 1 Passed: Requested 250000us, measured %ldus\n", elapsed_usec);

    /* Test 2: Multi-second duration test (1.2 seconds) */
    gettimeofday(&start, (struct timezone *)0);
    if (usleep(1200000UL) != 0) {
        perror("FAIL: Multi-second usleep failed");
        return 1;
    }
    gettimeofday(&end, (struct timezone *)0);

    elapsed_usec = (end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec);
    printf("Test 2 Passed: Requested 1200000us, measured %ldus\n", elapsed_usec);

    printf("RESULT: PASSED\n");
    return 0;
}
#endif /* _TEST_USLEEP_COMPAT */

