#ifndef MAP_COMPAT_H
#define MAP_COMPAT_H

#include <sys/types.h>

#ifndef MAP_ANON
#define MAP_ANON 0x20  /* For compatibility on older systems */
#endif
#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif


#endif /* MAP_COMPAT_H */
