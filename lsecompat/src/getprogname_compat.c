#include "getprogname_compat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#if defined(__GNUC__)
#  if defined(__ELF__) || defined(__solaris__) || defined(SOLARIS2)
#    include <sys/procfs.h>
     /* ELF systems: Use pragma weak */
#    pragma weak getprogname
#    pragma weak setprogname
#  elif defined(__aout__) || defined(sun) || defined(__sunos__)
     /* SunOS 4 / a.out systems: Standard declarations without weak attributes */
     const char *getprogname(void);
     void        setprogname(const char *name);
#  else
     /* Fallback for other GCC platforms supporting weak attributes */
     const char *getprogname(void)      __attribute__((weak));
     void        setprogname(const char *name) __attribute__((weak));
#  endif
#endif

/* Safely import __progname as a weak reference to avoid linker errors */
#if defined(__GNUC__)
extern char *__progname __attribute__((weak));
#else
extern char *__progname;
#endif

static const char *lse_explicit_progname = NULL;

void setprogname(const char *name) {
    const char *p = strrchr(name, '/');
    lse_explicit_progname = (p != NULL) ? p + 1 : name;
}

/* Self-contained PID-to-string conversion using native getpid() */
char *compat_pid_to_str(char *buf) {
    pid_t pid = getpid();
    char temp[32];
    int i = 0;
    int j = 0;

    if (pid == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return buf;
    }

    while (pid > 0) {
        temp[i++] = (char)('0' + (pid % 10));
        pid /= 10;
    }

    /* Reverse digits into output buffer */
    while (i > 0) {
        buf[j++] = temp[--i];
    }
    buf[j] = '\0';

    return buf;
}

const char *getprogname(void) {
#if defined(HAS_PROCFS_SVR4)
    static char static_progname[PRFNSZ + 1] = {0};
#else
    static char static_progname[32] = {0};
#endif
    static const char *progname_ptr = NULL;

    /* Return cached result if already determined */
    if (progname_ptr != NULL) {
        return progname_ptr;
    }

    /* Strategy 1: Explicit setprogname override */
    if (lse_explicit_progname != NULL && *lse_explicit_progname != '\0') {
        progname_ptr = lse_explicit_progname;
        return progname_ptr;
    }

    /* Strategy 2: Weak check for GCC __progname global symbol */
    if (&__progname != NULL && __progname != NULL && *__progname != '\0') {
        const char *p = strrchr(__progname, '/');
        progname_ptr = (p != NULL) ? p + 1 : __progname;
        return progname_ptr;
    }

#if defined(HAS_PROCFS_SVR4)
    /* Strategy 3: Solaris 2.5.1 SVR4 ProcFS ioctl */
    {
        prpsinfo_t psinfo;
        char proc_path[64];
        char pid_buf[32];
        int fd;

        strcpy(proc_path, "/proc/");
        compat_pid_to_str(pid_buf);
        strcat(proc_path, pid_buf);

        fd = open(proc_path, O_RDONLY);
        if (fd >= 0) {
            if (ioctl(fd, PIOCPSINFO, &psinfo) == 0) {
                if (psinfo.pr_fname[0] != '\0') {
                    strncpy(static_progname, psinfo.pr_fname, PRFNSZ);
                    static_progname[PRFNSZ] = '\0';
                    progname_ptr = static_progname;
                }
            }
            close(fd);
            if (progname_ptr != NULL) {
                return progname_ptr;
            }
        }
    }
#endif

    /* Strategy 4: Fallback default */
    strncpy(static_progname, "unknown", sizeof(static_progname) - 1);
    progname_ptr = static_progname;

    return progname_ptr;
}

/* =========================================================================
 * UNIT TEST BLOCK                                                         
 * Compile with: gcc -O2 -DTEST_GETPROGNAME_COMPAT getprogname_compat.c -o test_app
 * ========================================================================= */
#ifdef TEST_GETPROGNAME_COMPAT

int main(int argc, char *argv[]) {
    const char *pname1;
    const char *pname2;

    (void)argc;
    (void)argv;

    printf("[TEST] Querying getprogname()...\n");
    pname1 = getprogname();
    printf("[TEST] Detected program name (Call 1): '%s'\n", pname1);

    pname2 = getprogname();
    printf("[TEST] Detected program name (Call 2 - Cached): '%s'\n", pname2);

    if (pname1 != NULL && strcmp(pname1, "unknown") != 0) {
        printf("[TEST] SUCCESS: Successfully retrieved process name!\n");
        return 0;
    } else {
        printf("[TEST] WARNING: Fallback 'unknown' returned.\n");
        return 1;
    }
}

#endif /* TEST_GETPROGNAME_COMPAT */

