#ifndef SOCKET_COMPAT_H
#define SOCKET_COMPAT_H

#include <sys/types.h>

/* ====================================================================
 * SECTION 1: SYSTEM INCLUDES, TYPEDEFS, AND MACRO DEFINITIONS
 * ==================================================================== */

#include <sys/types.h>

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

/* POSIX.1-2001 / BSD socket flags missing or limited in Solaris 2.5.1 */

#ifndef MSG_WAITALL
#  define MSG_WAITALL   0x0    /* Don't block for full buffer; process available bytes */
#endif

#ifndef MSG_DONTWAIT
#  define MSG_DONTWAIT  0x0    /* Non-blocking I/O flag (use O_NONBLOCK via fcntl instead) */
#endif

#ifndef MSG_NOSIGNAL
#  define MSG_NOSIGNAL  0x0    /* Do not generate SIGPIPE on broken pipe (use signal(SIGPIPE, SIG_IGN)) */
#endif

#ifndef MSG_EOR
#  define MSG_EOR       0x8    /* End of record (often used in SOCK_SEQPACKET / OSI) */
#endif

#ifndef MSG_CONFIRM
#  define MSG_CONFIRM   0x0    /* Linux link-layer progress hint; no-op on SVR4 */
#endif

#ifndef MSG_MORE
#  define MSG_MORE      0x0    /* Linux TCP corking hint; no-op on SVR4 */
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

#endif /* SOCKET_COMPAT_H */
