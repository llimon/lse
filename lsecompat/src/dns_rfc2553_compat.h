#ifndef DNS_RFC2553_COMPAT_H
#define DNS_RFC2553_COMPAT_H

#include <sys/types.h>

#ifndef COMPAT_DNS_RFC2553_COMPAT_H
#define COMPAT_DNS_RFC2553_COMPAT_H

#include <sys/types.h>
#include <stdint.h>
#include <compat/socket_compat.h> /* KEEP */

#ifndef AF_INET
#define AF_INET 2
#endif

/* =========================================================================
 * RFC 2553 getnameinfo() Flags & String Buffer Limits (32-bit SPARC)
 * ========================================================================= */
#ifndef NI_MAXHOST
#define NI_MAXHOST      1025
#endif

#ifndef NI_MAXSERV
#define NI_MAXSERV      32
#endif

#ifndef NI_NUMERICHOST
#define NI_NUMERICHOST  0x0001
#endif

#ifndef NI_NUMERICSERV
#define NI_NUMERICSERV  0x0002
#endif

#ifndef NI_NOFQDN
#define NI_NOFQDN       0x0004
#endif

#ifndef NI_NAMEREQD
#define NI_NAMEREQD     0x0008
#endif

#ifndef NI_DGRAM
#define NI_DGRAM        0x0010
#endif

/* Standard RFC 2553 / RFC 3493 Error Codes */
#ifndef EAI_ADDRFAMILY
#define EAI_ADDRFAMILY  1 /* Address family for hostname not supported */
#define EAI_AGAIN       2 /* Temporary failure in name resolution */
#define EAI_BADFLAGS    3 /* Invalid value for ai_flags */
#define EAI_FAIL        4 /* Non-recoverable failure in name resolution */
#define EAI_FAMILY      5 /* ai_family not supported */
#define EAI_MEMORY      6 /* Memory allocation failure */
#define EAI_NODATA      7 /* No address associated with hostname */
#define EAI_NONAME      8 /* Name or service not known */
#define EAI_SERVICE     9 /* Servname not supported for ai_socktype */
#define EAI_SOCKTYPE   10 /* ai_socktype not supported */
#define EAI_SYSTEM     11 /* System error returned in errno */
#define EAI_BADHINTS   12 /* Invalid hints */
#define EAI_PROTOCOL   13 /* Resolved protocol is unknown */
#define EAI_MAX        14
#endif

/* AI Flags */
#ifndef AI_PASSIVE
#define AI_PASSIVE     0x0001
#define AI_CANONNAME   0x0002
#define AI_NUMERICHOST 0x0004
#define AI_NUMERICSERV 0x0008
#endif

/* Standard struct addrinfo definition */
#ifndef COMPAT_ADDRINFO_DEFINED
#define COMPAT_ADDRINFO_DEFINED
struct addrinfo {
    int              ai_flags;
    int              ai_family;
    int              ai_socktype;
    int              ai_protocol;
    size_t           ai_addrlen;
    char            *ai_canonname;
    struct sockaddr *ai_addr;
    struct addrinfo *ai_next;  /* Correct self-referential linked list pointer */
};
#endif /* COMPAT_ADDRINFO_DEFINED */

/* Multi-IP DNS Pool-Capable getaddrinfo */
#ifndef IPPROTO_UDP
#define IPPROTO_UDP 17
#endif
#ifndef IPPROTO_TCP
#define IPPROTO_TCP 6
#endif

void freeaddrinfo(struct addrinfo *res);
char *gai_strerror(int ecode);
int getaddrinfo(const char *nodename, const char *servname,
        const struct addrinfo *hints, struct addrinfo **res);
int getnameinfo(const struct sockaddr *sa, socklen_t salen,
                char *host, size_t hostlen,
                char *serv, size_t servlen, int flags);

#endif /* COMPAT_DNS_RFC2553_COMPAT_H */

#endif /* DNS_RFC2553_COMPAT_H */
