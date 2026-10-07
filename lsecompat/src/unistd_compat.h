#ifndef LSE_UNISTD_COMPAT_H
#define LSE_UNISTD_COMPAT_H

#include <unistd.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Solaris 2.5.1 libc.so.1 provides them, but /usr/include lacks the prototypes */
int gethostname(char *name, size_t len);
int mkstemp(char *template_str);

#ifdef __cplusplus
}
#endif

#endif /* LSE_UNISTD_COMPAT_H */
