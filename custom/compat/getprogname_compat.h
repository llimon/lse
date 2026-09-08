#ifndef GETPROGNAME_COMPAT_H
#define GETPROGNAME_COMPAT_H

#ifndef COMPAT_GETPROGNAME_COMPAT
#define COMPAT_GETPROGNAME_COMPAT 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/procfs.h>

/* Self-contained PID-to-string conversion using native getpid() */
static inline char *compat_pid_to_str(char *buf) {
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

static inline const char *getprogname(void) {
    static char static_progname[PRFNSZ + 1] = {0};
    static const char *progname_ptr = NULL;
    prpsinfo_t psinfo;
    char proc_path[64];
    char pid_buf[32];
    int fd;

    /* Return cached result if already determined */
    if (progname_ptr != NULL) {
        return progname_ptr;
    }

    /* Fallback default */
    strncpy(static_progname, "unknown", sizeof(static_progname) - 1);
    progname_ptr = static_progname;

    /* Construct "/proc/<pid>" manually using getpid() output */
    strcpy(proc_path, "/proc/");
    compat_pid_to_str(pid_buf);
    strcat(proc_path, pid_buf);

    fd = open(proc_path, O_RDONLY);
    if (fd < 0) {
        return progname_ptr;
    }

    /* Query Solaris 2.5.1 process information struct */
    if (ioctl(fd, PIOCPSINFO, &psinfo) == 0) {
        if (psinfo.pr_fname[0] != '\0') {
            strncpy(static_progname, psinfo.pr_fname, PRFNSZ);
            static_progname[PRFNSZ] = '\0';
        }
    }

    close(fd);
    return progname_ptr;
}

#endif /* HAVE_GETPROGNAME */

/* ========================================================================= */
/* UNIT TEST BLOCK                                                           */
/* Enable by compiling with: -DTEST_GETPROGNAME                              */
/* Example: gcc -O2 -DTEST_GETPROGNAME -I. getprogname_compat.h -o test_app  */
/* ========================================================================= */
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
        printf("[TEST] SUCCESS: Successfully retrieved process name via /proc!\n");
        return 0;
    } else {
        printf("[TEST] WARNING: Fallback 'unknown' returned.\n");
        return 1;
    }
}

#endif /* TEST_GETPROGNAME */

#endif /* GETPROGNAME_COMPAT_H */
