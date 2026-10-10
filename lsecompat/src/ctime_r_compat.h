/* In lsecompat.h */
#if defined(SOLARIS2_5_1) || defined(SOLARIS2_5)

#include <time.h>

/* Overload selector macro to handle both 2 and 3 argument calls */
#define SELECT_CTIME_R(_1, _2, _3, NAME, ...) NAME

#define ctime_r_2(clock, buf)        ctime_r((clock), (buf), sizeof(buf))
#define ctime_r_3(clock, buf, len)   ctime_r((clock), (buf), (len))

#undef ctime_r
#define ctime_r(...) SELECT_CTIME_R(__VA_ARGS__, ctime_r_3, ctime_r_2)(__VA_ARGS__)

#endif
