#ifndef LSE_GETLINE_COMPAT_H
#define LSE_GETLINE_COMPAT_H

#include <stdio.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

ssize_t getline(char **lineptr, size_t *n, FILE *stream);
ssize_t getdelim(char **lineptr, size_t *n, int delim, FILE *stream);

#ifdef __cplusplus
}
#endif

#endif /* LSE_GETLINE_COMPAT_H */
