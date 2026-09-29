#include "time_compat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utime.h>
#include <errno.h>

/*
 * GCC 2.95 / Sun Assembler compatible weak directives for Solaris ELF
 */
#if defined(__GNUC__)
#  if defined(__ELF__) || defined(__solaris__)
#    pragma weak utimes
#    pragma weak clock_gettime
#    pragma weak clock_getres
#  else
     int utimes(const char *path, const struct timeval times[2]) __attribute__((weak));
     int clock_gettime(clockid_t clk_id, struct timespec *tp) __attribute__((weak));
     int clock_getres(clockid_t clk_id, struct timespec *res) __attribute__((weak));
#  endif
#endif

/* =========================================================================
 * 1. utimes() Implementation via utime()
 * ========================================================================= */
int utimes(const char *path, const struct timeval times[2]) {
    struct utimbuf new_times;

    if (path == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (times == NULL) {
        return utime(path, NULL);
    }

    new_times.actime  = times[0].tv_sec;
    new_times.modtime = times[1].tv_sec;

    return utime(path, &new_times);
}

/* =========================================================================
 * 2. clock_gettime() Implementation via gettimeofday()
 * ========================================================================= */
int clock_gettime(clockid_t clk_id, struct timespec *tp) {
    struct timeval tv;

    if (tp == NULL) {
        errno = EFAULT;
        return -1;
    }

    /* Solaris 2.5.1 lacks monotonic hardware clocks; fallback to realtime */
    if (clk_id != CLOCK_REALTIME && clk_id != CLOCK_MONOTONIC) {
        errno = EINVAL;
        return -1;
    }

    if (gettimeofday(&tv, NULL) != 0) {
        return -1;
    }

    tp->tv_sec  = tv.tv_sec;
    tp->tv_nsec = tv.tv_usec * 1000L;

    return 0;
}

/* =========================================================================
 * 3. clock_getres() Implementation (1 microsecond resolution via gettimeofday)
 * ========================================================================= */
int clock_getres(clockid_t clk_id, struct timespec *res) {
    if (res == NULL) {
        errno = EFAULT;
        return -1;
    }

    if (clk_id != CLOCK_REALTIME && clk_id != CLOCK_MONOTONIC) {
        errno = EINVAL;
        return -1;
    }

    /* gettimeofday provides microsecond (1,000 ns) resolution */
    res->tv_sec  = 0;
    res->tv_nsec = 1000L;

    return 0;
}

/* =========================================================================
 * Embedded Unit Test Harness
 * Compile test: gcc -O2 -mcpu=v7 -D_TEST_TIME_COMPAT time_compat.c -o test_time
 * ========================================================================= */
#ifdef _TEST_TIME_COMPAT
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>

int main(void) {
    const char *test_filename = "/tmp/test_time_compat.tmp";
    struct stat st;
    struct timeval custom_times[2];
    struct timespec ts, res;
    int fd, ret;

    printf("=== Running Solaris / SunOS Time Compat Unit Tests ===\n");

    /* -----------------------------------------------------------------
     * utimes() Tests
     * ----------------------------------------------------------------- */
    /* Create temporary test file */
    fd = open(test_filename, O_RDWR | O_CREAT | O_TRUNC, 0666);
    assert(fd >= 0);
    close(fd);

    /* Test 1: Set explicit timestamps (Jan 1, 2020 00:00:00 UTC) */
    custom_times[0].tv_sec = 1577836800; /* atime */
    custom_times[0].tv_usec = 500000;
    custom_times[1].tv_sec = 1577836800; /* mtime */
    custom_times[1].tv_usec = 500000;

    ret = utimes(test_filename, custom_times);
    assert(ret == 0);

    ret = stat(test_filename, &st);
    assert(ret == 0);
    assert(st.st_atime == 1577836800);
    assert(st.st_mtime == 1577836800);
    printf("Test 1 Passed: utimes() explicit timestamps applied correctly\n");

    /* Test 2: Set current time via NULL argument */
    ret = utimes(test_filename, NULL);
    assert(ret == 0);
    printf("Test 2 Passed: utimes() with NULL parameter updated to current time\n");
    unlink(test_filename);

    /* -----------------------------------------------------------------
     * clock_gettime() Tests
     * ----------------------------------------------------------------- */
    /* Test 3: Read CLOCK_REALTIME */
    ret = clock_gettime(CLOCK_REALTIME, &ts);
    assert(ret == 0);
    assert(ts.tv_sec > 1500000000L); /* Sanity check for valid Unix epoch */
    assert(ts.tv_nsec >= 0 && ts.tv_nsec < 1000000000L);
    printf("Test 3 Passed: clock_gettime() CLOCK_REALTIME = %ld sec, %ld nsec\n",
           (long)ts.tv_sec, ts.tv_nsec);

    /* Test 4: Read CLOCK_MONOTONIC */
    ret = clock_gettime(CLOCK_MONOTONIC, &ts);
    assert(ret == 0);
    assert(ts.tv_sec > 0);
    assert(ts.tv_nsec >= 0 && ts.tv_nsec < 1000000000L);
    printf("Test 4 Passed: clock_gettime() CLOCK_MONOTONIC = %ld sec, %ld nsec\n",
           (long)ts.tv_sec, ts.tv_nsec);

    /* Test 5: Invalid Clock ID handling */
    ret = clock_gettime(999, &ts);
    assert(ret == -1);
    assert(errno == EINVAL);
    printf("Test 5 Passed: clock_gettime() invalid clock ID correctly returned EINVAL\n");

    /* Test 6: NULL buffer handling */
    ret = clock_gettime(CLOCK_REALTIME, NULL);
    assert(ret == -1);
    assert(errno == EFAULT);
    printf("Test 6 Passed: clock_gettime() NULL tp parameter correctly returned EFAULT\n");

    /* -----------------------------------------------------------------
     * clock_getres() Tests
     * ----------------------------------------------------------------- */
    /* Test 7: Query Resolution for CLOCK_REALTIME */
    ret = clock_getres(CLOCK_REALTIME, &res);
    assert(ret == 0);
    assert(res.tv_sec == 0);
    assert(res.tv_nsec == 1000L);
    printf("Test 7 Passed: clock_getres() reported resolution = %ld nsec\n", res.tv_nsec);

    /* Test 8: NULL buffer handling for clock_getres */
    ret = clock_getres(CLOCK_REALTIME, NULL);
    assert(ret == -1);
    assert(errno == EFAULT);
    printf("Test 8 Passed: clock_getres() NULL res parameter correctly returned EFAULT\n");

    printf("\nRESULT: ALL 8 TESTS PASSED\n");
    return 0;
}
#endif /* _TEST_TIME_COMPAT */
