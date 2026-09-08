#ifndef COMPAT_WCSSTR_COMPAT_H
#define COMPAT_WCSSTR_COMPAT_H

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

/**
 * Static inline wcsstr implementation for legacy SunOS 5.5.1
 */
static inline wchar_t *wcsstr_compat(const wchar_t *haystack, const wchar_t *needle) {
    if (!haystack) {
        return NULL;
    }

    /* An empty needle matches at haystack head */
    if (!*needle) {
        return (wchar_t *)haystack;
    }

    while (*haystack) {
        const wchar_t *h = haystack;
        const wchar_t *n = needle;

        while (*n && (*h == *n)) {
            h++;
            n++;
        }

        if (!*n) {
            return (wchar_t *)haystack;
        }

        haystack++;
    }

    return NULL;
}

#ifndef COMPAT_WCSSTR_COMPAT
#define wcsstr wcsstr_compat
#endif

#endif /* COMPAT_WCSSTR_COMPAT_H */

/* =========================================================================
 *  Embedded Unit Test Harness
 *  Compile with: gcc -x c -std=gnu99 -D_TEST_WCSSTR_COMPAT=1 wcsstr_compat.h -o test_wcsstr
 * ========================================================================= */
#ifdef _TEST_WCSSTR_COMPAT
int main(void) {
    const wchar_t *haystack = L"Hello, SunOS 5.5.1 world!";
    const wchar_t *needle_found = L"SunOS";
    const wchar_t *needle_absent = L"Solaris 11";
    const wchar_t *needle_empty = L"";

    printf("=== Running Solaris 2.5.1 wcsstr_compat Unit Test ===\n");

    /* Test 1: Match middle */
    wchar_t *res1 = wcsstr_compat(haystack, needle_found);
    if (!res1 || res1 != (haystack + 7)) {
        printf("FAIL: Basic substring match failed\n");
        return 1;
    }
    printf("Test 1 Passed: Found 'SunOS' at position 7\n");

    /* Test 2: Substring not present */
    wchar_t *res2 = wcsstr_compat(haystack, needle_absent);
    if (res2 != NULL) {
        printf("FAIL: Returned non-NULL for absent substring\n");
        return 1;
    }
    printf("Test 2 Passed: Correctly returned NULL for absent substring\n");

    /* Test 3: Empty string match */
    wchar_t *res3 = wcsstr_compat(haystack, needle_empty);
    if (res3 != haystack) {
        printf("FAIL: Empty needle did not return haystack pointer\n");
        return 1;
    }
    printf("Test 3 Passed: Empty needle returned haystack start\n");

    printf("RESULT: PASSED\n");
    return 0;
}
#endif /* _TEST_WCSSTR_COMPAT */
