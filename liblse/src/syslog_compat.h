#include <syslog.h>

/* SunOS 4.1.x Prototypes Missing from System Headers */
#if defined(sun) && !defined(SOLARIS2) && !defined(__solaris__)

/* 1. Syslog function prototypes */
void openlog(const char *ident, int logopt, int facility);
void syslog(int priority, const char *message, ...);
void closelog(void);
int  setlogmask(int maskpri);

/* 2. String fallback prototypes (prevents pointer-from-integer warnings) */
char *strerror(int errnum);

#endif
