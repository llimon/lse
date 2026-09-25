#include "mbstowcs_compat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <errno.h>
#  include <wchar.h>

#if defined(__GNUC__)
#  if defined(__ELF__) || defined(__solaris__)
#    pragma weak mbrtowc
#    pragma weak mbstowcs
#  else
     size_t mbrtowc(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps) __attribute__((weak));
     size_t mbstowcs(wchar_t *dest, const char *src, size_t n)            __attribute__((weak));
#  endif
#endif

size_t mbrtowc(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps) {
    (void)ps;

    if (s == NULL) {
        if (pwc) *pwc = 0;
        return 0;
    }

    if (*s == '\0') {
        if (pwc) *pwc = 0;
        return 0;
    }

    unsigned char byte = (unsigned char)s[0];

    /* ASCII 1-byte */
    if (byte <= 0x7F) {
        if (n < 1) return (size_t)-2;
        if (pwc) *pwc = (wchar_t)byte;
        return 1;
    }

    /* 2-byte UTF-8 */
    if ((byte & 0xE0) == 0xC0) {
        if (n < 2) return (size_t)-2;
        unsigned char byte2 = (unsigned char)s[1];
        if ((byte2 & 0xC0) != 0x80) {
            errno = EILSEQ;
            return (size_t)-1;
        }
        if (pwc) *pwc = (wchar_t)(((byte & 0x1F) << 6) | (byte2 & 0x3F));
        return 2;
    }

    /* 3-byte UTF-8 */
    if ((byte & 0xF0) == 0xE0) {
        if (n < 3) return (size_t)-2;
        unsigned char byte2 = (unsigned char)s[1];
        unsigned char byte3 = (unsigned char)s[2];
        if ((byte2 & 0xC0) != 0x80 || (byte3 & 0xC0) != 0x80) {
            errno = EILSEQ;
            return (size_t)-1;
        }
        if (pwc) *pwc = (wchar_t)(((byte & 0x0F) << 12) | ((byte2 & 0x3F) << 6) | (byte3 & 0x3F));
        return 3;
    }

    errno = EILSEQ;
    return (size_t)-1;
}

size_t mbstowcs(wchar_t *dest, const char *src, size_t n) {
    mbstate_t state;
    size_t converted_count = 0;
    const char *p_src = src;
    wchar_t *p_dest = dest;

    memset(&state, 0, sizeof(state));

    if (src == NULL) {
        errno = EINVAL;
        return (size_t)-1;
    }

    while (1) {
        wchar_t wc;
        size_t len = mbrtowc(&wc, p_src, 4, &state);

        if (len == 0) {
            if (dest != NULL && converted_count < n) {
                *p_dest = 0;
            }
            break;
        } else if (len == (size_t)-1 || len == (size_t)-2) {
            return (size_t)-1;
        } else {
            if (dest != NULL) {
                if (converted_count >= n) {
                    break;
                }
                *p_dest = wc;
                p_dest++;
            }
            p_src += len;
            converted_count++;
        }
    }

    return converted_count;
}

/* =========================================================================
 *  Embedded Unit Test Harness
 *  Compile test: gcc -O2 -mcpu=v7 -D_TEST_MBSTOWCS_COMPAT mbstowcs_compat.c -o test_mbstowcs
 * ========================================================================= */
#ifdef _TEST_MBSTOWCS_COMPAT
int main(void) {
    if (setlocale(LC_ALL, "en_US") == NULL) {
        setlocale(LC_ALL, "C");
    }

    const char *mb_string = "Hello world!";
    wchar_t wc_buffer[50];
    size_t converted;

    printf("=== Running Universal mbstowcs Unit Test ===\n");
    printf("Original multibyte string: \"%s\"\n", mb_string);

    /* 1. Test standard string conversion */
    converted = mbstowcs(wc_buffer, mb_string, 50);

    if (converted == (size_t)-1) {
        perror("FAIL: mbstowcs conversion failed");
        return 1;
    }

    printf("Converted %zu wide characters.\n", converted);

    /* 2. Test NULL dest mode (Length query) */
    size_t query_len = mbstowcs(NULL, mb_string, 0);
    printf("Length query via NULL dest: %zu (Expected: 12)\n", query_len);

    if (converted == 12 && query_len == 12) {
        printf("RESULT: PASSED\n");
        return 0;
    }

    printf("RESULT: FAILED\n");
    return 1;
}
#endif /* _TEST_MBSTOWCS_COMPAT */

