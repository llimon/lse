#ifndef _SPARC32_INTTYPE_H
#define _SPARC32_INTTYPE_H

#include <stdint.h>
#include <sys/types.h>

#if defined(__sun__) && defined(__sparc__)
/* 32-bit SPARC (Sun-4m / Solaris 2.5.1 ABI) */
#define PRIdPTR "ld"
#define PRIuPTR "lu"
#define PRIuMAX "llu"
#define PRIoMAX "llo"
#define PRIdMAX "lld"
#define PRIxMAX "llx"
#define PRIXMAX "llX"

#define PRId32 "d"
#define PRIu32 "u"
#define PRIx32 "x"
#define PRId64 "lld"
#define PRIu64 "llu"
#define PRIx64 "llx"

#ifndef SCNuMAX
#define SCNuMAX "llu"
#endif

#ifndef SCNdMAX
#define SCNdMAX "lld"
#endif

#ifndef SCNxMAX
#define SCNxMAX "llx"
#endif

#elif defined(__x86_64__) || defined(__aarch64__)
/* 64-bit systems */
#define PRIdPTR "ld"
#define PRIuPTR "lu"
#define PRIuMAX "llu"
#define PRIoMAX "llo"
#define PRIdMAX "lld"

#define PRId32 "d"
#define PRIu32 "u"
#define PRIx32 "x"
#define PRId64 "ld"
#define PRIu64 "lu"
#define PRIx64 "lx"

#define SCNuMAX "llu"
#define SCNdMAX "lld"
#define SCNxMAX "llx"

#else
/* Standard 32-bit Fallback */
#define PRIdPTR "d"
#define PRIuPTR "u"
#define PRIuMAX "llu"
#define PRIoMAX "llo"
#define PRIdMAX "lld"

#define PRId32 "d"
#define PRIu32 "u"
#define PRIx32 "x"
#define PRId64 "lld"
#define PRIu64 "llu"
#define PRIx64 "llx"

#define SCNuMAX "llu"
#define SCNdMAX "lld"
#define SCNxMAX "llx"

#endif

/* C99 string-to-intmax conversion prototypes */
#ifdef __cplusplus
extern "C" {
#endif

extern intmax_t  strtoimax(const char *nptr, char **endptr, int base);
extern uintmax_t strtoumax(const char *nptr, char **endptr, int base);

#ifdef __cplusplus
}
#endif


#endif /* _SPARC32_INTTYPE_H */
