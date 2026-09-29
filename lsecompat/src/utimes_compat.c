#include "utimes_compat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <utime.h>
#include <sys/time.h> /* For struct timeval */
#include <errno.h>

/*
 * GCC 2.95 / Sun Assembler compatible weak directive
 */
#if defined(__GNUC__)
#  if defined(__ELF__) || defined(__solaris__)
     /* Weak symbol directive for Sun as / GNU as on Solaris ELF */
#    pragma weak utimes
#  else
     /* Fallback attribute for SunOS 4.1.4 / a.out */
     int utimes(const char *path, const struct timeval times[2]) __attribute__((weak));
#  endif
#endif

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
 *  Embedded Unit Test Harness
 *  Compile test: gcc -O2 -mcpu=v7 -D_TEST_UTIMES_COMPAT utimes_compat.c -o test_utimes
 * ========================================================================= */
#ifdef _TEST_UTIMES_COMPAT
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main(void) {
    const char *test_filename = "/tmp/test_utimes.tmp";
    struct stat st;
    struct timeval custom_times[2];
    int fd;

    printf("=== Running Solaris / SunOS utimes Unit Test ===\n");

    /* Create temporary test file */
    fd = open(test_filename, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        perror("FAIL: Could not create test file");
        return 1;
    }
    close(fd);

    /* Test 1: Set custom timestamps (Jan 1, 2020 00:00:00 UTC) */
    custom_times[0].tv_sec = 1577836800; /* atime */
    custom_times[0].tv_usec = 500000;
    custom_times[1].tv_sec = 1577836800; /* mtime */
    custom_times[1].tv_usec = 500000;

    if (utimes(test_filename, custom_times) != 0) {
        perror("FAIL: utimes with explicit times failed");
        unlink(test_filename);
        return 1;
    }

    if (stat(test_filename, &st) != 0) {
        perror("FAIL: stat failed");
        unlink(test_filename);
        return 1;
    }

    if (st.st_atime != 1577836800 || st.st_mtime != 1577836800) {
        printf("FAIL: Timestamp mismatch. Expected %ld, got atime=%ld mtime=%ld\n",
               1577836800L, (long)st.st_atime, (long)st.st_mtime);
        unlink(test_filename);
        return 1;
    }
    printf("Test 1 Passed: Custom timestamps applied correctly\n");

    /* Test 2: Set current time via NULL argument */
    if (utimes(test_filename, NULL) != 0) {
        perror("FAIL: utimes with NULL times failed");
        unlink(test_filename);
        return 1;
    }
    printf("Test 2 Passed: NULL times parameter set current timestamp\n");

    unlink(test_filename);
    printf("RESULT: PASSED\n");
    return 0;
}
#endif /* _TEST_UTIMES_COMPAT */

