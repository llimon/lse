#include "wcsstr_compat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#  include <wchar.h>

#if defined(__GNUC__)
#  if defined(__ELF__) || defined(__solaris__)
#    pragma weak wcsstr
#  else
     wchar_t *wcsstr(const wchar_t *haystack, const wchar_t *needle) __attribute__((weak));
#  endif
#endif

wchar_t *wcsstr(const wchar_t *haystack, const wchar_t *needle) {
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

/* =========================================================================
 *  Embedded Unit Test Harness
 *  Compile test: gcc -O2 -mcpu=v7 -D_TEST_WCSSTR_COMPAT wcsstr_compat.c -o test_wcsstr
 * ========================================================================= */
#ifdef _TEST_WCSSTR_COMPAT
int main(void) {
    static const wchar_t haystack[] = {'H','e','l','l','o',',',' ','S','u','n','O','S','!',0};
    static const wchar_t needle_found[] = {'S','u','n','O','S',0};
    static const wchar_t needle_absent[] = {'S','o','l','a','r','i','s',0};
    static const wchar_t needle_empty[] = {0};

    printf("=== Running Universal wcsstr Unit Test ===\n");

    /* Test 1: Match middle */
    wchar_t *res1 = wcsstr(haystack, needle_found);
    if (!res1 || res1 != (haystack + 7)) {
        printf("FAIL: Basic substring match failed\n");
        return 1;
    }
    printf("Test 1 Passed: Found 'SunOS' at position 7\n");

    /* Test 2: Substring not present */
    wchar_t *res2 = wcsstr(haystack, needle_absent);
    if (res2 != NULL) {
        printf("FAIL: Returned non-NULL for absent substring\n");
        return 1;
    }
    printf("Test 2 Passed: Correctly returned NULL for absent substring\n");

    /* Test 3: Empty string match */
    wchar_t *res3 = wcsstr(haystack, needle_empty);
    if (res3 != haystack) {
        printf("FAIL: Empty needle did not return haystack pointer\n");
        return 1;
    }
    printf("Test 3 Passed: Empty needle returned haystack start\n");

    printf("RESULT: PASSED\n");
    return 0;
}
#endif /* _TEST_WCSSTR_COMPAT */

