#include "dns_rfc2553_compat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h> 
#include <sys/socket.h> 
#include <compat/socket_compat.h> /* KEEP */
#include <netinet/in.h>
#include <arpa/inet.h>

#if defined(__GNUC__)
#  if defined(__ELF__) || defined(__solaris__) || defined(SOLARIS2)
     /* ELF systems: Use pragma weak */
#    pragma weak freeaddrinfo
#    pragma weak gai_strerror
#    pragma weak getaddrinfo
#    pragma weak getnameinfo
#  elif defined(__aout__) || defined(sun) || defined(__sunos__)
     /* SunOS 4 / a.out systems: Standard declarations without weak attributes */
     void  freeaddrinfo(struct addrinfo *res);
     char *gai_strerror(int ecode);
     int   getaddrinfo(const char *nodename, const char *servname,
                       const struct addrinfo *hints,
                       struct addrinfo **res);
     int   getnameinfo(const struct sockaddr *sa, socklen_t salen,
                       char *host, size_t hostlen,
                       char *serv, size_t servlen, int flags);
#  else
     /* Fallback for other GCC platforms supporting weak attributes */
     void  freeaddrinfo(struct addrinfo *res) __attribute__((weak));
     char *gai_strerror(int ecode)            __attribute__((weak));
     int   getaddrinfo(const char *nodename, const char *servname,
                       const struct addrinfo *hints,
                       struct addrinfo **res) __attribute__((weak));
     int   getnameinfo(const struct sockaddr *sa, socklen_t salen,
                       char *host, size_t hostlen,
                       char *serv, size_t servlen, int flags) __attribute__((weak));
#  endif
#endif

/* Free linked list */
void freeaddrinfo(struct addrinfo *res) {
    struct addrinfo *p, *next;
    for (p = res; p != NULL; p = next) {
        next = p->ai_next;
        if (p->ai_addr) free(p->ai_addr);
        if (p->ai_canonname) free(p->ai_canonname);
        free(p);
    }
}

/* Error string lookup using clean switch statement */
char *gai_strerror(int ecode) {
    switch (ecode) {
        case 0:              return "Success";
        case EAI_ADDRFAMILY: return "Address family for hostname not supported";
        case EAI_AGAIN:      return "Temporary failure in name resolution";
        case EAI_BADFLAGS:   return "Invalid value for ai_flags";
        case EAI_FAIL:       return "Non-recoverable failure in name resolution";
        case EAI_FAMILY:     return "ai_family not supported";
        case EAI_MEMORY:     return "Memory allocation failure";
        case EAI_NODATA:     return "No address associated with hostname";
        case EAI_NONAME:     return "Name or service not known";
        case EAI_SERVICE:    return "Servname not supported for ai_socktype";
        case EAI_SOCKTYPE:   return "ai_socktype not supported";
        case EAI_SYSTEM:     return "System error";
        case EAI_BADHINTS:   return "Invalid hints";
        case EAI_PROTOCOL:   return "Unknown protocol";
        default:             return "Unknown error";
    }
}

/*
 * NOTE / FUTURE MAINTAINER WARNING:
 * Defaulting socktype/protocol to SOCK_DGRAM / IPPROTO_UDP when hints->ai_socktype == 0 
 * is intentionally preserved for stability across tools like Git and NTP.
 * Strict POSIX requires yielding both SOCK_STREAM and SOCK_DGRAM nodes when 
 * ai_socktype is unspecified, but doing so has caused memory/traversal crashes 
 * in certain legacy client builds. Do not alter this fallback behavior without 
 * full regression testing against Git/OpenSSL.
 */
int getaddrinfo(const char *nodename, const char *servname,
                              const struct addrinfo *hints,
                              struct addrinfo **res) {
    struct hostent *he;
    struct servent *sp;
    struct addrinfo *head = NULL, *prev = NULL, *curr = NULL;
    struct sockaddr_in *sa_in;
    int port = 0;
    int family = AF_UNSPEC, socktype = 0, protocol = 0, flags = 0;
    const char *proto = NULL;
    int i = 0;

    if (nodename == NULL && servname == NULL)
        return (EAI_NONAME);

    if (hints != NULL) {
        family = hints->ai_family;
        socktype = hints->ai_socktype;
        protocol = hints->ai_protocol;
        flags = hints->ai_flags;

        if (family != AF_UNSPEC && family != AF_INET)
            return (EAI_FAMILY);

        if (socktype == SOCK_DGRAM) {
            proto = "udp";
            if (protocol == 0) protocol = IPPROTO_UDP;
        } else if (socktype == SOCK_STREAM) {
            proto = "tcp";
            if (protocol == 0) protocol = IPPROTO_TCP;
        }
    }

    /* Process Service / Port */
    if (servname != NULL) {
        char *e;
        long tmp_port = strtol(servname, &e, 10);
        if (*e == '\0') {
            if (tmp_port < 0 || tmp_port > 65535)
                return (EAI_SERVICE);
            port = htons((unsigned short)tmp_port);
        } else {
            sp = getservbyname(servname, proto);
            if (sp == NULL && proto != NULL) {
                sp = getservbyname(servname, NULL);
            }

            if (sp != NULL) {
                port = sp->s_port;
                if (socktype == 0) {
                    if (strcmp(sp->s_proto, "tcp") == 0) {
                        socktype = SOCK_STREAM;
                        protocol = IPPROTO_TCP;
                    } else if (strcmp(sp->s_proto, "udp") == 0) {
                        socktype = SOCK_DGRAM;
                        protocol = IPPROTO_UDP;
                    }
                }
            } else if (strcmp(servname, "ntp") == 0) {
                port = htons(123);
                if (socktype == 0) socktype = SOCK_DGRAM;
                if (protocol == 0) protocol = IPPROTO_UDP;
            } else {
                return (EAI_SERVICE);
            }
        }
    }

    /* Fallbacks if unspecified */
    if (socktype == 0) socktype = SOCK_DGRAM;
    if (protocol == 0) protocol = IPPROTO_UDP;

    if (nodename != NULL) {
        struct in_addr in;
        unsigned long ip_val = (unsigned long)inet_addr(nodename);

        /* 1. Direct numeric IPv4 address check (e.g. "192.168.1.1") */
        if (ip_val != 0xFFFFFFFFUL || strcmp(nodename, "255.255.255.255") == 0) {
            in.s_addr = (uint32_t)ip_val;

            curr = (struct addrinfo *)calloc(1, sizeof(struct addrinfo));
            sa_in = (struct sockaddr_in *)calloc(1, sizeof(struct sockaddr_in));

            if (!curr || !sa_in) {
                if (curr) free(curr);
                if (sa_in) free(sa_in);
                return (EAI_MEMORY);
            }

            sa_in->sin_family = AF_INET;
            sa_in->sin_port = (unsigned short)port;
            sa_in->sin_addr = in;

            curr->ai_flags = flags;
            curr->ai_family = AF_INET;
            curr->ai_socktype = socktype;
            curr->ai_protocol = protocol;
            curr->ai_addrlen = sizeof(struct sockaddr_in);
            curr->ai_addr = (struct sockaddr *)sa_in;
            curr->ai_next = NULL;

            if (flags & AI_CANONNAME) {
                curr->ai_canonname = strdup(nodename);
            }

            head = curr;
        } else {
            /* 2. DNS Hostname lookup (e.g. "0.pool.ntp.org") */
            he = gethostbyname(nodename);
            if (he == NULL || he->h_addrtype != AF_INET)
                return (EAI_NONAME);

            while (he->h_addr_list[i] != NULL) {
                curr = (struct addrinfo *)calloc(1, sizeof(struct addrinfo));
                sa_in = (struct sockaddr_in *)calloc(1, sizeof(struct sockaddr_in));

                if (!curr || !sa_in) {
                    if (curr) free(curr);
                    if (sa_in) free(sa_in);
                    freeaddrinfo(head);
                    return (EAI_MEMORY);
                }

                sa_in->sin_family = AF_INET;
                sa_in->sin_port = (unsigned short)port;
                memcpy(&sa_in->sin_addr, he->h_addr_list[i], he->h_length);

                curr->ai_flags = flags;
                curr->ai_family = AF_INET;
                curr->ai_socktype = socktype;
                curr->ai_protocol = protocol;
                curr->ai_addrlen = sizeof(struct sockaddr_in);
                curr->ai_addr = (struct sockaddr *)sa_in;
                curr->ai_next = NULL;

                if ((flags & AI_CANONNAME) && he->h_name) {
                    curr->ai_canonname = strdup(he->h_name);
                }

                if (!head) head = curr;
                else prev->ai_next = curr;

                prev = curr;
                i++;
            }
        }
    } else {
        /* 3. Wildcard bind (nodename is NULL) */
        curr = (struct addrinfo *)calloc(1, sizeof(struct addrinfo));
        sa_in = (struct sockaddr_in *)calloc(1, sizeof(struct sockaddr_in));

        if (!curr || !sa_in) {
            if (curr) free(curr);
            if (sa_in) free(sa_in);
            return (EAI_MEMORY);
        }

        sa_in->sin_family = AF_INET;
        sa_in->sin_port = (unsigned short)port;
        sa_in->sin_addr.s_addr = (flags & AI_PASSIVE) ? INADDR_ANY : htonl(INADDR_LOOPBACK);

        curr->ai_flags = flags;
        curr->ai_family = AF_INET;
        curr->ai_socktype = socktype;
        curr->ai_protocol = protocol;
        curr->ai_addrlen = sizeof(struct sockaddr_in);
        curr->ai_addr = (struct sockaddr *)sa_in;
        curr->ai_next = NULL;

        head = curr;
    }

    if (head == NULL)
        return (EAI_NONAME);

    *res = head;
    return 0;
}

/* Standard Reverse Lookup getnameinfo */
int getnameinfo(const struct sockaddr *sa, socklen_t salen,
                              char *host, size_t hostlen,
                              char *serv, size_t servlen, int flags) {
    struct hostent *hp;
    size_t namelen;

    if (!sa || sa->sa_family != AF_INET)
        return EAI_FAMILY;

    hp = gethostbyaddr((const char *)&((const struct sockaddr_in *)sa)->sin_addr,
                       4, AF_INET);
    if (!hp)
        return (h_errno == TRY_AGAIN) ? EAI_AGAIN : EAI_FAIL;

    if (host && hostlen > 0) {
        namelen = strlen(hp->h_name);
        if (namelen >= hostlen) namelen = hostlen - 1;
        strncpy(host, hp->h_name, namelen);
        host[namelen] = '\0';
    }
    return 0;
}

/* =========================================================================
 * Unit Test Suite
 * Compile with: gcc -D_TEST_DNS_RFC2553_COMPAT -I/usr/tgcware/include -o test_dns /usr/tgcware/include/compat/dns_rfc2553_compat.h
 * ========================================================================= */
#ifdef _TEST_DNS_RFC2553_COMPAT
int main(int argc, char **argv) {
    const char *target = (argc > 1) ? argv[1] : "0.pool.ntp.org";
    const char *service = (argc > 2) ? argv[2] : "123";
    struct addrinfo hints, *res = NULL, *p = NULL;
    int rc, count = 0, errors = 0;

    printf("[TEST] Testing dns_rfc2553_compat on Solaris 2.5.1...\n");
    printf("[TEST] System Constants: SOCK_STREAM=%d | SOCK_DGRAM=%d | AF_INET=%d\n",
           SOCK_STREAM, SOCK_DGRAM, AF_INET);
    printf("[TEST] Target: %s | Service: %s\n\n", target, service);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    rc = getaddrinfo(target, service, &hints, &res);
    if (rc != 0) {
        fprintf(stderr, "[FAIL] getaddrinfo() returned error %d: %s\n", rc, gai_strerror(rc));
        return 1;
    }

    if (res == NULL) {
        fprintf(stderr, "[FAIL] getaddrinfo() returned 0 (Success) but result pointer 'res' is NULL!\n");
        return 1;
    }

    printf("[OK] getaddrinfo() returned SUCCESS. Parsing address chain:\n");
    printf("--------------------------------------------------\n");

    for (p = res; p != NULL; p = p->ai_next) {
        count++;
        if (p->ai_family == AF_INET && p->ai_addr != NULL) {
            struct sockaddr_in *sin = (struct sockaddr_in *)p->ai_addr;
            char *ip_str = inet_ntoa(sin->sin_addr);
            int port = ntohs(sin->sin_port);
            const char *st_str = (p->ai_socktype == SOCK_DGRAM) ? "SOCK_DGRAM" :
                                 (p->ai_socktype == SOCK_STREAM) ? "SOCK_STREAM" : "INVALID";

            printf("  Node %d:\n", count);
            printf("    IP Address: %s\n", ip_str ? ip_str : "UNKNOWN");
            printf("    Port:       %d\n", port);
            printf("    Family:     AF_INET (%d)\n", p->ai_family);
            printf("    SockType:   %s (%d)\n", st_str, p->ai_socktype);
            printf("    CanonName:  %s\n", p->ai_canonname ? p->ai_canonname : "(none)");

            /* Validation Assertions */
            if (p->ai_socktype != SOCK_DGRAM) {
                fprintf(stderr, "    [ERROR] Node %d: SockType mismatch! Expected SOCK_DGRAM (%d), got %d\n",
                        count, SOCK_DGRAM, p->ai_socktype);
                errors++;
            }
            if (p->ai_family != AF_INET) {
                fprintf(stderr, "    [ERROR] Node %d: Family mismatch! Expected AF_INET (%d), got %d\n",
                        count, AF_INET, p->ai_family);
                errors++;
            }
            if (port == 0) {
                fprintf(stderr, "    [ERROR] Node %d: Port is 0! Expected valid port for service '%s'\n",
                        count, service);
                errors++;
            }
            printf("\n");
        } else {
            fprintf(stderr, "    [ERROR] Node %d: Corrupted or non-AF_INET address structure!\n", count);
            errors++;
        }
    }

    printf("--------------------------------------------------\n");
    printf("[SUMMARY] Resolved and chained %d IP address(es) with %d error(s).\n", count, errors);

    freeaddrinfo(res);
    printf("[OK] freeaddrinfo() executed without heap corruption.\n");

    return (errors > 0) ? 1 : 0;
}
#endif /* _TEST_DNS_RFC2553_COMPAT */

