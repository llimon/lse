#ifndef COMPAT_SOCKET_COMPAT_H
#define COMPAT_SOCKET_COMPAT_H

/* ====================================================================
 * SECTION 1: SYSTEM INCLUDES, TYPEDEFS, AND MACRO DEFINITIONS
 * ==================================================================== */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h> 

/* Shutdown constants for shutdown(2) */
#ifndef SHUT_RD
#define SHUT_RD   0
#endif

#ifndef SHUT_WR
#define SHUT_WR   1
#endif

#ifndef SHUT_RDWR
#define SHUT_RDWR 2
#endif

/* Solaris 2.5.1 socklen_t fallback */
#ifndef socklen_t
#ifndef SOCKLEN_T
#ifndef _SOCKLEN_T
typedef int socklen_t;
#define SOCKLEN_T
#define _SOCKLEN_T
#endif
#endif
#endif

/* Solaris 2.5.1 in_addr_t fallback */
#ifndef IN_ADDR_T
#ifndef _IN_ADDR_T
typedef unsigned long in_addr_t;
#define IN_ADDR_T
#define _IN_ADDR_T
#endif
#endif

/* Solaris 2.5.1 INET_ADDRSTRLEN fallback */
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 46
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Standard POSIX prototypes */
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
int inet_pton(int af, const char *src, void *dst);

#ifdef __cplusplus
}
#endif

/* ====================================================================
 * SECTION 2: IMPLEMENTATION & WEAK LINKING
 * ==================================================================== */
/* BEGIN_IMPL */

#if defined(__GNUC__)
#  if defined(__ELF__) || defined(__solaris__)
#    pragma weak inet_ntop
#    pragma weak inet_pton
#  else
     const char *inet_ntop(int af, const void *src, char *dst, socklen_t size) __attribute__((weak));
     int inet_pton(int af, const char *src, void *dst) __attribute__((weak));
#  endif
#endif

/* Solaris 2.5.1 inet_ntop IPv4 fallback */
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size) {
    char *addr;
    if (af != AF_INET) {
        errno = EAFNOSUPPORT;
        return NULL;
    }
    addr = inet_ntoa(*(const struct in_addr *)src);
    if (!addr) return NULL;
    if (strlen(addr) >= (size_t)size) {
        errno = ENOSPC;
        return NULL;
    }
    strcpy(dst, addr);
    return dst;
}

/* Solaris 2.5.1 inet_pton IPv4 fallback with strict octet checking */
int inet_pton(int af, const char *src, void *dst) {
    int a, b, c, d;
    char dummy;
    in_addr_t addr;

    if (af != AF_INET) {
        errno = EAFNOSUPPORT;
        return -1;
    }

    if (!src || !dst) return 0;

    /* Ensure exactly 4 integer octets matching 0-255 */
    if (sscanf(src, "%d.%d.%d.%d%c", &a, &b, &c, &d, &dummy) != 4) {
        return 0;
    }

    if (a < 0 || a > 255 || b < 0 || b > 255 ||
        c < 0 || c > 255 || d < 0 || d > 255) {
        return 0;
    }

    addr = inet_addr(src);
    if (addr == (in_addr_t)-1 && strcmp(src, "255.255.255.255") != 0) {
        return 0;
    }

    ((struct in_addr *)dst)->s_addr = addr;
    return 1;
}

/* Embedded Standalone Unit Test */
#ifdef _TEST_SOCKET_COMPAT_CUSTOM

#include <assert.h>

static void verify_constants_and_types(void) {
    printf("1. Verifying socket constants and type definitions...\n");

    /* Shutdown Constants */
    assert(SHUT_RD == 0);
    assert(SHUT_WR == 1);
    assert(SHUT_RDWR == 2);

    /* Buffer Length Constants */
    assert(INET_ADDRSTRLEN == 16);
    assert(INET6_ADDRSTRLEN == 46);

    /* Type Sizes */
    assert(sizeof(socklen_t) == sizeof(int));
    assert(sizeof(in_addr_t) == sizeof(unsigned long));

    printf("   -> Socket constants and fallback types verified.\n");
}

static void verify_inet_pton(void) {
    printf("2. Verifying inet_pton() string-to-binary conversions...\n");

    struct in_addr in;
    int res;

    /* Valid IPv4 Loopback */
    res = inet_pton(AF_INET, "127.0.0.1", &in);
    assert(res == 1);
    assert(in.s_addr == inet_addr("127.0.0.1"));

    /* Valid IPv4 Broadcast */
    res = inet_pton(AF_INET, "255.255.255.255", &in);
    assert(res == 1);
    assert(in.s_addr == inet_addr("255.255.255.255"));

    /* Invalid IP string should return 0 */
    res = inet_pton(AF_INET, "999.999.999.999", &in);
    assert(res == 0);

    /* Unsupported Address Family should return -1 and set EAFNOSUPPORT */
    errno = 0;
    res = inet_pton(AF_UNSPEC, "127.0.0.1", &in);
    assert(res == -1);
    assert(errno == EAFNOSUPPORT);

    printf("   -> inet_pton() tests passed successfully!\n");
}

static void verify_inet_ntop(void) {
    printf("3. Verifying inet_ntop() binary-to-string conversions...\n");

    struct in_addr in;
    char buf[INET_ADDRSTRLEN];
    const char *ptr;
    char small_buf[4];

    /* Convert binary 192.168.1.1 to string */
    in.s_addr = inet_addr("192.168.1.1");
    ptr = inet_ntop(AF_INET, &in, buf, sizeof(buf));
    assert(ptr != NULL);
    assert(strcmp(buf, "192.168.1.1") == 0);

    /* Small buffer failure test (should return NULL and set ENOSPC) */
    errno = 0;
    ptr = inet_ntop(AF_INET, &in, small_buf, sizeof(small_buf));
    assert(ptr == NULL);
    assert(errno == ENOSPC);

    /* Unsupported Address Family test */
    errno = 0;
    ptr = inet_ntop(AF_UNSPEC, &in, buf, sizeof(buf));
    assert(ptr == NULL);
    assert(errno == EAFNOSUPPORT);

    printf("   -> inet_ntop() tests passed successfully!\n");
}

int main(void) {
    printf("==================================================\n");
    printf(" Running POSIX Socket Compatibility Test Suite    \n");
    printf("==================================================\n");
    verify_constants_and_types();
    verify_inet_pton();
    verify_inet_ntop();
    printf("\n>>> System-wide socket_compat suite FULLY VERIFIED. <<<\n");
    return 0;
}

#endif /* _TEST_SOCKET_COMPAT_CUSTOM */
/* END_IMPL */

#endif /* COMPAT_SOCKET_COMPAT_H */
