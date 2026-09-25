#ifndef _STDINT_H
#define _STDINT_H

/* Custom C99 stdint.h for SunOS 4.1.x & Solaris 2.x / SPARC V8 (32-bit) */

#ifdef _MSC_VER
    #if defined(_M_AMD64)
      typedef unsigned __int64 uintptr_t;
    #else
      typedef unsigned long uintptr_t;
    #endif

    typedef __int8           int8_t;
    typedef __int16          int16_t;
    typedef __int32          int32_t;
    typedef __int64          int64_t;
    typedef unsigned __int8  uint8_t;
    typedef unsigned __int16 uint16_t;
    typedef unsigned __int32 uint32_t;
    typedef unsigned __int64 uint64_t;
#elif defined(__GNUC__)

    #include <sys/types.h>
    #include <limits.h>

    /* Include sys/synch.h ONLY on Solaris 2.x / SVR4 (SunOS 4 lacks this header) */
    #if defined(SOLARIS2) || defined(__solaris__) || defined(__SVR4)
    #  include <sys/synch.h>
    #endif

    #if defined(__UINTPTR_TYPE__)
        typedef __UINTPTR_TYPE__ uintptr_t;
    #else
        typedef unsigned long uintptr_t;
    #endif

    #if defined(__INTPTR_TYPE__)
        typedef __INTPTR_TYPE__ intptr_t;
    #else
        typedef long intptr_t;
    #endif

    /* Standard fixed-width integer types */
    typedef signed char        int8_t;
    typedef short              int16_t;
    typedef int                int32_t;
    typedef long long          int64_t;

    #ifndef _UINT8_T
    #define _UINT8_T
    typedef unsigned char      uint8_t;
    #endif

    typedef unsigned short     uint16_t;

    #ifndef _UINT32_T
    #define _UINT32_T
    typedef unsigned int       uint32_t;
    #endif

    #ifndef _UINT64_T
    #define _UINT64_T
    typedef unsigned long long uint64_t;
    #endif

    /* Greatest-width integer types */
    #ifndef _INTMAX_T
    #define _INTMAX_T
    typedef long long          intmax_t;
    #endif

    #ifndef _UINTMAX_T
    #define _UINTMAX_T
    typedef unsigned long long uintmax_t;
    #endif

    /* Fast integer types */
    #ifndef _INT_FAST8_T
    #define _INT_FAST8_T
    typedef int8_t             int_fast8_t;
    #endif

    #ifndef _UINT_FAST8_T
    #define _UINT_FAST8_T
    typedef uint8_t            uint_fast8_t;
    #endif

    #ifndef _INT_FAST16_T
    #define _INT_FAST16_T
    typedef int                int_fast16_t;
    #endif

    #ifndef _UINT_FAST16_T
    #define _UINT_FAST16_T
    typedef unsigned int       uint_fast16_t;
    #endif

    #ifndef _INT_FAST32_T
    #define _INT_FAST32_T
    typedef int32_t            int_fast32_t;
    #endif

    #ifndef _UINT_FAST32_T
    #define _UINT_FAST32_T
    typedef uint32_t           uint_fast32_t;
    #endif

    #ifndef _INT_FAST64_T
    #define _INT_FAST64_T
    typedef int64_t            int_fast64_t;
    #endif

    #ifndef _UINT_FAST64_T
    #define _UINT_FAST64_T
    typedef uint64_t           uint_fast64_t;
    #endif

    /* 8-bit & 16-bit limits */
    #ifndef INT8_MAX
    #define INT8_MAX           127
    #endif
    #ifndef INT8_MIN
    #define INT8_MIN           (-127 - 1)
    #endif
    #ifndef UINT8_MAX
    #define UINT8_MAX          255
    #endif

    #ifndef INT16_MAX
    #define INT16_MAX          32767
    #endif
    #ifndef INT16_MIN
    #define INT16_MIN          (-32767 - 1)
    #endif
    #ifndef UINT16_MAX
    #define UINT16_MAX         65535U
    #endif

    /* 32-bit limits */
    #ifndef INT32_MAX
    #define INT32_MAX          2147483647L
    #endif
    #ifndef INT32_MIN
    #define INT32_MIN          (-2147483647L - 1L)
    #endif
    #ifndef UINT32_MAX
    #define UINT32_MAX         4294967295U
    #endif

    /* 64-bit limits with fallbacks for limits.h */
    #ifndef LLONG_MAX
    #define LLONG_MAX          9223372036854775807LL
    #endif
    #ifndef LLONG_MIN
    #define LLONG_MIN          (-LLONG_MAX - 1LL)
    #endif
    #ifndef ULLONG_MAX
    #define ULLONG_MAX         18446744073709551615ULL
    #endif

    #ifndef INT64_MAX
    #define INT64_MAX          LLONG_MAX
    #endif
    #ifndef INT64_MIN
    #define INT64_MIN          LLONG_MIN
    #endif
    #ifndef UINT64_MAX
    #define UINT64_MAX         ULLONG_MAX
    #endif

    /* Greatest-width integer limits */
    #ifndef INTMAX_MAX
    #define INTMAX_MAX         LLONG_MAX
    #endif
    #ifndef INTMAX_MIN
    #define INTMAX_MIN         LLONG_MIN
    #endif
    #ifndef UINTMAX_MAX
    #define UINTMAX_MAX        ULLONG_MAX
    #endif

    #ifndef SIZE_MAX
    #define SIZE_MAX           UINT32_MAX
    #endif

#endif /* __GNUC__ */

#endif /* _STDINT_H */
