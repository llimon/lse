#include <signal.h>

/* SunOS 4.1.x Signal Flag Fallbacks */
#if defined(sun) && !defined(SOLARIS2) && !defined(__solaris__)

#  ifndef SA_RESTART
#    define SA_RESTART 0
#  endif

#  ifndef SA_NOCLDSTOP
#    define SA_NOCLDSTOP 0
#  endif

#endif
