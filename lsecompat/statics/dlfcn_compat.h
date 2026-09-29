#ifndef COMPAT_DLFCN_COMPAT_H
#define COMPAT_DLFCN_COMPAT_H

/* Force inclusion of system dlfcn.h first to pull in Dl_info and prototypes */
#include <dlfcn.h>

/* Supply missing POSIX flags for Solaris 2.5.1 */
#ifndef RTLD_LOCAL
#  define RTLD_LOCAL 0x00000
#endif

#endif /* COMPAT_DLFCN_COMPAT_H */
