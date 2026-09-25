/* 2. Map POSIX file sync flags to standard O_SYNC on SunOS 4 */

#  ifndef _SSIZE_T
#    define _SSIZE_T
     typedef int ssize_t;
#  endif

#  ifndef O_RSYNC
#    define O_RSYNC O_SYNC
#  endif
#  ifndef O_DSYNC
#    define O_DSYNC O_SYNC
#  endif
