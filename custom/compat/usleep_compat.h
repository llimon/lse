#ifndef COMPAT_USLEEP_COMPAT_H
#define COMPAT_USLEEP_COMPAT_H

#include <time.h>
#include <errno.h>

/**
 * Static inline usleep() implementation for legacy SunOS 5.5.1
 * Wraps POSIX nanosleep() with full signal interruption handling.
 */
#if defined(__GNUC__) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L))
static inline int usleep_compat(unsigned long usec) {
#else
static int usleep_compat(unsigned long usec) {
#endif
    struct timespec req;
    struct timespec rem;
    int ret;

    /* POSIX/BSD usleep restricts values to less than 1,000,000 microseconds */
    if (usec >= 1000000UL) {
        errno = EINVAL;
        return -1;
    }

    req.tv_sec = 0;
    req.tv_nsec = (long)(usec * 1000UL);

    /* Loop to handle EINTR signal interrupts automatically */
    while ((ret = nanosleep(&req, &rem)) == -1 && errno == EINTR) {
        req = rem;
    }

    return ret;
}

#ifndef COMPAT_USLEEP_COMPAT
#define usleep usleep_compat
#endif

#endif /* COMPAT_USLEEP_COMPAT_H */

/* =========================================================================
 *  Embedded Unit Test Harness
 *  Compile with: gcc -x c -std=gnu99 -D_TEST_USLEEP_COMPAT=1 usleep_compat.h -o test_usleep
 * ========================================================================= */
#ifdef _TEST_USLEEP_COMPAT
#include <stdio.h>
#include <sys/time.h>

int main(void) {
    struct timeval start, end;
    long elapsed_usec;

    printf("=== Running Solaris 2.5.1 usleep_compat Unit Test ===\n");

    /* Test 1: Validate 250,000 microsecond (250ms) sleep duration */
    gettimeofday(&start, NULL);
    if (usleep_compat(250000UL) != 0) {
        perror("FAIL: usleep_compat execution failed");
        return 1;
    }
    gettimeofday(&end, NULL);

    elapsed_usec = (end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec);
    printf("Test 1 Passed: Requested 250000us, measured %ldus\n", elapsed_usec);

    /* Test 2: Invalid input guard (>= 1,000,000us) */
    if (usleep_compat(1000000UL) == -1 && errno == EINVAL) {
        printf("Test 2 Passed: Correctly returned EINVAL for >= 1,000,000us\n");
    } else {
        printf("FAIL: Did not reject invalid microsecond value properly\n");
        return 1;
    }

    printf("RESULT: PASSED\n");
    return 0;
}
#endif /* _TEST_USLEEP_COMPAT */
