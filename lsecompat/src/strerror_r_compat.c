#include "strerror_r_compat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
 * GCC / Solaris ELF weak directive:
 * On Solaris 2.5.1 ELF, wrap with weak pragma so if a future system library 
 * or newer OS provides native strerror_r, it overrides this shim cleanly.
 */
#if defined(__GNUC__) && (defined(__ELF__) || defined(__solaris__) || defined(SOLARIS2))
#  pragma weak strerror_r
#endif

int strerror_r(int errnum, char *buf, size_t buflen) {
    char *errstr;

    if (buf == NULL || buflen == 0) {
        return EINVAL;
    }

    errstr = strerror(errnum);
    if (errstr == NULL) {
        snprintf(buf, buflen, "Unknown error %d", errnum);
        return EINVAL;
    }

    if (strlen(errstr) >= buflen) {
        strncpy(buf, errstr, buflen - 1);
        buf[buflen - 1] = '\0';
        return ERANGE;
    }

    strcpy(buf, errstr);
    return 0;
}
