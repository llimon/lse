#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <inttypes.h>
#include <stdint.h>
#include <limits.h>
#include <ctype.h>
#include <errno.h>
#include "strtoimax_compat.h"

#if defined(__GNUC__)
#  if defined(__ELF__) || defined(__solaris__) || defined(SOLARIS2)
     /* ELF systems: Use pragma weak for dynamic symbol overrides */
#    pragma weak strtoimax
#    pragma weak strtoumax
#  elif defined(__aout__) || defined(sun) || defined(__sunos__)
     /* SunOS 4 / a.out systems: Weak symbols are unsupported by the linker.
      * Provide standard prototype declarations without weak attributes. */
     intmax_t  strtoimax(const char *nptr, char **endptr, int base);
     uintmax_t strtoumax(const char *nptr, char **endptr, int base);
#  else
     /* Fallback for other GCC platforms supporting weak attributes */
     intmax_t  strtoimax(const char *nptr, char **endptr, int base)  __attribute__((weak));
     uintmax_t strtoumax(const char *nptr, char **endptr, int base) __attribute__((weak));
#  endif
#endif

intmax_t strtoimax(const char *nptr, char **endptr, int base) {
    const char *s = nptr;
    uintmax_t acc = 0;
    int sign = 1;
    int any = 0;
    uintmax_t maxval = (sign < 0) ? ((uintmax_t)INTMAX_MAX + 1ULL)
                                  : (uintmax_t)INTMAX_MAX;
    uintmax_t cutoff = maxval / (uintmax_t)base;
    uintmax_t cutlim = maxval % (uintmax_t)base;

    if ((base < 0 || base == 1 || base > 36) && base != 0) {
        if (endptr) *endptr = (char *)nptr;
        errno = EINVAL;
        return 0;
    }

    while (isspace((unsigned char)*s)) s++;

    if (*s == '-') {
        sign = -1;
        s++;
    } else if (*s == '+') {
        s++;
    }

    /* Check for 0x / 0X hex prefix */
    if ((base == 0 || base == 16) &&
        s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        unsigned char next_ch = (unsigned char)s[2];
        if (isdigit(next_ch) || (isalpha(next_ch) && tolower(next_ch) <= 'f')) {
            s += 2;
            base = 16;
        }
    }

    if (base == 0) {
        base = (s[0] == '0') ? 8 : 10;
    }


    while (*s) {
        unsigned char ch = (unsigned char)*s;
        int digit;

        if (isdigit(ch)) {
            digit = ch - '0';
        } else if (isalpha(ch)) {
            digit = tolower(ch) - 'a' + 10;
        } else {
            break;
        }

        if (digit >= base) break;

        if (any < 0 || acc > cutoff || (acc == cutoff && (uintmax_t)digit > cutlim)) {
            any = -1;
        } else {
            any = 1;
            acc = acc * (uintmax_t)base + (uintmax_t)digit;
        }
        s++;
    }

    if (endptr) *endptr = (char *)(any ? s : nptr);

    if (any < 0) {
        errno = ERANGE;
        return (sign < 0) ? INTMAX_MIN : INTMAX_MAX;
    }

    return (sign < 0) ? -(intmax_t)acc : (intmax_t)acc;
}

uintmax_t strtoumax(const char *nptr, char **endptr, int base) {
    const char *s = nptr;
    uintmax_t acc = 0;
    int sign = 1;
    int any = 0;
    uintmax_t cutoff = UINTMAX_MAX / (uintmax_t)base;
    uintmax_t cutlim = UINTMAX_MAX % (uintmax_t)base;

    if ((base < 0 || base == 1 || base > 36) && base != 0) {
        if (endptr) *endptr = (char *)nptr;
        errno = EINVAL;
        return 0;
    }

    while (isspace((unsigned char)*s)) s++;

    if (*s == '-') {
        sign = -1;
        s++;
    } else if (*s == '+') {
        s++;
    }

    /* Check for 0x / 0X hex prefix */
    if ((base == 0 || base == 16) &&
        s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        unsigned char next_ch = (unsigned char)s[2];
        if (isdigit(next_ch) || (isalpha(next_ch) && tolower(next_ch) <= 'f')) {
            s += 2;
            base = 16;
        }
    }

    if (base == 0) {
        base = (s[0] == '0') ? 8 : 10;
    }


    while (*s) {
        unsigned char ch = (unsigned char)*s;
        int digit;

        if (isdigit(ch)) {
            digit = ch - '0';
        } else if (isalpha(ch)) {
            digit = tolower(ch) - 'a' + 10;
        } else {
            break;
        }

        if (digit >= base) break;

        if (any < 0 || acc > cutoff || (acc == cutoff && (uintmax_t)digit > cutlim)) {
            any = -1;
        } else {
            any = 1;
            acc = acc * (uintmax_t)base + (uintmax_t)digit;
        }
        s++;
    }

    if (endptr) *endptr = (char *)(any ? s : nptr);

    if (any < 0) {
        errno = ERANGE;
        return UINTMAX_MAX;
    }

    return (sign < 0) ? (uintmax_t)(-(intmax_t)acc) : acc;
}

/* Embedded Standalone Unit Test */
#ifdef _TEST_STRTOIMAX_CUSTOM

#include <inttypes.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

static void verify_header_constants(void) {
    printf("1. Verifying C99 stdint/inttypes constants and type sizes...\n");

    /* 1a. Type Size Verification */
    assert(sizeof(intmax_t) == 8);
    assert(sizeof(uintmax_t) == 8);

    /* 1b. Limits Values Verification */
    assert(INTMAX_MAX == 9223372036854775807LL);
    assert(INTMAX_MIN == (-9223372036854775807LL - 1LL));
    assert(UINTMAX_MAX == 18446744073709551615ULL);

    /* 1c. Format Specifier Verification */
    char buf[128];
    sprintf(buf, "%" PRIdMAX " %" PRIuMAX " %" PRIxMAX,
            INTMAX_MAX, UINTMAX_MAX, (uintmax_t)0x1A3F);
    assert(strcmp(buf, "9223372036854775807 18446744073709551615 1a3f") == 0);

    printf("   -> Header constants and types are valid 64-bit C99 definitions.\n");
}

static void run_strtoimax_tests(void) {
    char *endptr;
    intmax_t res_i;
    uintmax_t res_u;

    printf("2. Testing strtoimax / strtoumax execution...\n");

    /* Test 1: Basic Decimal Parsing */
    errno = 0;
    res_i = strtoimax("  1234567890123", &endptr, 10);
    assert(res_i == 1234567890123LL);
    assert(*endptr == '\0');
    assert(errno == 0);

    /* Test 2: Negative Base 10 */
    errno = 0;
    res_i = strtoimax(" -987654321", &endptr, 10);
    assert(res_i == -987654321LL);
    assert(*endptr == '\0');
    assert(errno == 0);

    /* Test 3: Hex Auto-Detection (base 0) - 'z' stops hex parsing cleanly */
    errno = 0;
    res_i = strtoimax("0x1A3Fztra", &endptr, 0);
    assert(res_i == 0x1A3F);
    assert(strcmp(endptr, "ztra") == 0);
    assert(errno == 0);

    /* Test 4: Octal Auto-Detection (base 0) */
    errno = 0;
    res_i = strtoimax("0755", &endptr, 0);
    assert(res_i == 0755);
    assert(*endptr == '\0');
    assert(errno == 0);

    /* Test 5: Positive Overflow (INTMAX_MAX) */
    errno = 0;
    res_i = strtoimax("999999999999999999999999999999999", &endptr, 10);
    assert(res_i == INTMAX_MAX);
    assert(errno == ERANGE);

    /* Test 6: Negative Overflow (INTMAX_MIN) */
    errno = 0;
    res_i = strtoimax("-999999999999999999999999999999999", &endptr, 10);
    assert(res_i == INTMAX_MIN);
    assert(errno == ERANGE);

    /* Test 7: POSIX Unsigned Negative Wrap-Around (strtoumax) */
    errno = 0;
    res_u = strtoumax(" -1", &endptr, 10);
    assert(res_u == UINTMAX_MAX);
    assert(*endptr == '\0');
    assert(errno == 0);

    /* Test 8: Invalid Base Error */
    errno = 0;
    res_i = strtoimax("123", &endptr, 37);
    assert(res_i == 0);
    assert(strcmp(endptr, "123") == 0);
    assert(errno == EINVAL);

    printf("   -> All parser execution tests passed successfully!\n");
}

int main(void) {
    verify_header_constants();
    run_strtoimax_tests();
    printf("\n>>> System-wide C99 inttypes / strtoimax suite FULLY VERIFIED. <<<\n");
    return 0;
}

#endif /* _TEST_STRTOIMAX_CUSTOM */

