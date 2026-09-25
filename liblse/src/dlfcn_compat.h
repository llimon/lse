#ifndef DLFCN_COMPAT_H
#define DLFCN_COMPAT_H

#include <sys/types.h>

#ifndef COMPAT_DLFCN_COMPAT_H
#define COMPAT_DLFCN_COMPAT_H

/* Force inclusion of system dlfcn.h first to pull in Dl_info and prototypes */

/* Supply missing POSIX flags for Solaris 2.5.1 */
#ifndef RTLD_LOCAL
#  define RTLD_LOCAL 0x00000
#endif

#endif /* COMPAT_DLFCN_COMPAT_H */

#endif /* DLFCN_COMPAT_H */
