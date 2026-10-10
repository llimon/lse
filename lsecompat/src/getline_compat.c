#include "getline_compat.h"
#include <stdlib.h>
#include <errno.h>

#if defined(__GNUC__)
#  if defined(__ELF__) || defined(__solaris__) || defined(SOLARIS2)
     /* ELF systems: Use pragma weak */
#    pragma weak getdelim
#    pragma weak getline
#  elif defined(__aout__) || defined(sun) || defined(__sunos__)
     /* SunOS 4 / a.out systems: Standard declarations without weak attributes */
      ssize_t getdelim(char **lineptr, size_t *n, int delim, FILE *stream);
      ssize_t getline(char **lineptr, size_t *n, FILE *stream);

#  else
      /* Fallback for other GCC platforms supporting weak attributes */
      ssize_t getdelim(char **lineptr, size_t *n, int delim, FILE *stream) __attribute__((weak)); 
      ssize_t getline(char **lineptr, size_t *n, FILE *stream) __attribute__((weak));

#  endif
#endif



ssize_t getdelim(char **lineptr, size_t *n, int delim, FILE *stream) {
    char *ptr, *eptr;
    int c;

    if (lineptr == NULL || n == NULL || stream == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (*lineptr == NULL || *n == 0) {
        *n = 128;
        *lineptr = malloc(*n);
        if (*lineptr == NULL) {
            errno = ENOMEM;
            return -1;
        }
    }

    ptr = *lineptr;
    eptr = ptr + *n;

    while ((c = fgetc(stream)) != EOF) {
        if (ptr + 1 >= eptr) {
            size_t new_size = *n * 2;
            ssize_t off = ptr - *lineptr;
            char *new_ptr = realloc(*lineptr, new_size);
            if (new_ptr == NULL) {
                errno = ENOMEM;
                return -1;
            }
            *lineptr = new_ptr;
            *n = new_size;
            ptr = *lineptr + off;
            eptr = *lineptr + *n;
        }

        *ptr++ = (char)c;
        if (c == delim) {
            break;
        }
    }

    if (c == EOF && ptr == *lineptr) {
        return -1;
    }

    *ptr = '\0';
    return (ssize_t)(ptr - *lineptr);
}

ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
    return getdelim(lineptr, n, '\n', stream);
}


/* =========================================================================
 * EMBEDDED UNIT TEST SUITE
 *  build :  gcc -g -D_TEST_GETLINE_COMPAT -o test_getline_compat getline_compat.c
 * ========================================================================= */
#ifdef _TEST_GETLINE_COMPAT

#include <assert.h>
#include <string.h>

static void test_invalid_args(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t ret;

    printf("\n--- Test: Invalid Arguments Guard ---\n");

    ret = getline(NULL, &len, stdin);
    printf("[TEST] getline(NULL, &len, stdin) -> return: %ld (errno: %d)\n", (long)ret, errno);
    assert(ret == -1 && errno == EINVAL);

    ret = getline(&line, NULL, stdin);
    printf("[TEST] getline(&line, NULL, stdin) -> return: %ld (errno: %d)\n", (long)ret, errno);
    assert(ret == -1 && errno == EINVAL);

    ret = getline(&line, &len, NULL);
    printf("[TEST] getline(&line, &len, NULL)  -> return: %ld (errno: %d)\n", (long)ret, errno);
    assert(ret == -1 && errno == EINVAL);
}

static void test_basic_line_reading(void) {
    FILE *tmp = tmpfile();
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    printf("\n--- Test: Basic Line Reading ---\n");
    assert(tmp != NULL);

    fputs("Hello Solaris 2.5.1\nSecond Line\nNoNewlineAtEnd", tmp);
    rewind(tmp);

    /* Test Line 1 */
    nread = getline(&line, &len, tmp);
    printf("[READ Line 1] return: %ld, buf_size: %lu, content: %s", (long)nread, (unsigned long)len, line);
    assert(nread == 20);
    assert(strcmp(line, "Hello Solaris 2.5.1\n") == 0);

    /* Test Line 2 */
    nread = getline(&line, &len, tmp);
    printf("[READ Line 2] return: %ld, buf_size: %lu, content: %s", (long)nread, (unsigned long)len, line);
    assert(nread == 12);
    assert(strcmp(line, "Second Line\n") == 0);

    /* Test Line 3 (EOF without trailing newline) */
    nread = getline(&line, &len, tmp);
    printf("[READ Line 3] return: %ld, buf_size: %lu, content: %s\n", (long)nread, (unsigned long)len, line);
    assert(nread == 14);
    assert(strcmp(line, "NoNewlineAtEnd") == 0);

    /* Test EOF */
    nread = getline(&line, &len, tmp);
    printf("[READ EOF]    return: %ld\n", (long)nread);
    assert(nread == -1);

    free(line);
    fclose(tmp);
}

static void test_custom_delimiter(void) {
    FILE *tmp = tmpfile();
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    printf("\n--- Test: Custom Delimiter (':') ---\n");
    assert(tmp != NULL);

    fputs("root:x:0:0:SuperUser:/root:/bin/sh", tmp);
    rewind(tmp);

    nread = getdelim(&line, &len, ':', tmp);
    printf("[READ Delim]  return: %ld, content: \"%s\"\n", (long)nread, line);
    assert(nread == 5);
    assert(strcmp(line, "root:") == 0);

    free(line);
    fclose(tmp);
}

static void test_dynamic_realloc(void) {
    FILE *tmp = tmpfile();
    char *line = NULL;
    size_t len = 16; /* Force small initial buffer to trigger realloc */
    ssize_t nread;
    char long_string[512];

    printf("\n--- Test: Dynamic Buffer Reallocation ---\n");
    assert(tmp != NULL);

    memset(long_string, 'A', 500);
    long_string[500] = '\n';
    long_string[501] = '\0';

    fputs(long_string, tmp);
    rewind(tmp);

    line = (char *)malloc(len);
    printf("[INIT Buffer] initial capacity: %lu bytes\n", (unsigned long)len);

    nread = getline(&line, &len, tmp);
    printf("[REALLOC]     return: %ld, new capacity: %lu bytes\n", (long)nread, (unsigned long)len);

    assert(nread == 501);
    assert(len >= 501);
    assert(strcmp(line, long_string) == 0);

    free(line);
    fclose(tmp);
}

int main(void) {
    printf("====================================================\n");
    printf("Running getline_compat unit tests (SunOS 5.5.1)...\n");
    printf("====================================================\n");

    test_invalid_args();
    test_basic_line_reading();
    test_custom_delimiter();
    test_dynamic_realloc();

    printf("\n====================================================\n");
    printf("ALL TESTS PASSED SUCCESSFULLY!\n");
    printf("====================================================\n");
    return 0;
}

#endif /* _TEST_GETLINE_COMPAT */
