#ifndef COMPAT_MBSTOWCS_COMPAT_H
#define COMPAT_MBSTOWCS_COMPAT_H

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>
#include <errno.h>

#ifndef COMPAT_MBSTATE_T_COMPAT
typedef struct {
    int __opaque[4];
} mbstate_t;
#endif

static inline size_t mbrtowc_compat(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps) {
    (void)ps;

    if (s == NULL) {
        if (pwc) *pwc = 0;
        return 0;
    }

    if (*s == '\0') {
        if (pwc) *pwc = L'\0';
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

static inline size_t mbstowcs_compat(wchar_t *dest, const char *src, size_t n) {
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
        size_t len = mbrtowc_compat(&wc, p_src, 4, &state);

        if (len == 0) {
            if (dest != NULL && converted_count < n) {
                *p_dest = L'\0';
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

/* Optional macro overrides for legacy source patches */
#ifndef COMPAT_MBSTOWCS_COMPAT
#define mbstowcs mbstowcs_compat
#endif

#ifndef COMPAT_MBROTWEC_COMPAT
#define mbrtowc mbrtowc_compat
#endif

#endif /* COMPAT_MBSTOWCS_COMPAT_H */


/* =========================================================================
 *  Embedded Unit Test Harness
 *  Compile with: gcc -x c -std=gnu99 -D_TEST_MBSTOWCS_COMPAT=1 compat/mbstowcs_compat.h -o test_mbstowcs
 * ========================================================================= */
#ifdef _TEST_MBSTOWCS_COMPAT
int main(void) {
    /* Set locale to base C/en_US locale available on SunOS 5.5.1 */
    if (setlocale(LC_ALL, "en_US") == NULL) {
        if (setlocale(LC_ALL, "C") == NULL) {
            perror("FAIL: setlocale");
            return 1;
        }
    }

    const char *mb_string = "Hello world!";
    wchar_t wc_buffer[50];
    size_t converted;

    printf("=== Running Solaris 2.5.1 mbstowcs_compat Unit Test ===\n");
    printf("Original multibyte string: \"%s\"\n", mb_string);

    /* 1. Test standard string conversion */
    converted = mbstowcs_compat(wc_buffer, mb_string, 50);

    if (converted == (size_t)-1) {
        perror("FAIL: mbstowcs_compat conversion failed");
        return 1;
    }

    printf("Converted %zu wide characters.\n", converted);
    printf("Wide character string: \"");
    for (size_t i = 0; i < converted; i++) {
        putwchar(wc_buffer[i]);
    }
    printf("\"\n");

    /* 2. Test NULL dest mode (Buffer length query used by Python/Readline) */
    size_t query_len = mbstowcs_compat(NULL, mb_string, 0);
    printf("Length query via NULL dest: %zu (Expected: 12)\n", query_len);

    if (converted == 12 && query_len == 12) {
        printf("RESULT: PASSED\n");
        return 0;
    }

    printf("RESULT: FAILED\n");
    return 1;
}
#endif /* _TEST_MBSTOWCS_COMPAT */

/*
 * Upstream Readline Patch Notes:
 * Inside _rl_mb_strcaseeqn(), declare local conversion state variables:
 *   mbstate_t ps1;
 *   mbstate_t ps2;
 *   const char *s2;
 *   int flags;
 */
