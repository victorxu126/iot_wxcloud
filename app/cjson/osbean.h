#ifndef __OSBEAN_H
#define __OSBEAN_H


#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN    4321
#define __PDP_ENDIAN    3412

#define BYTE_ORDER LITTLE_ENDIAN

#   ifndef __INT_MAX__
#    define __INT_MAX__ 2147483647
#   endif
#   undef INT_MIN
#   define INT_MIN (-INT_MAX-1)
#   undef INT_MAX
#   define INT_MAX __INT_MAX__

#define DBL_EPSILON 2.2204460492503131E-16

double floor(double x);
double pow(double x, double y);
double fabs(double x);


#define LWIP_PLATFORM_HTONS(_n)  ((uint16_t)((((_n) & 0xff) << 8) | (((_n) >> 8) & 0xff)))
#define LWIP_PLATFORM_HTONL(_n)  ((uint32_t)( (((_n) & 0xff) << 24) | (((_n) & 0xff00) << 8) | (((_n) >> 8)  & 0xff00) | (((_n) >> 24) & 0xff) ))

#define lwip_htons(x) LWIP_PLATFORM_HTONS(x)
#define lwip_ntohs(x) LWIP_PLATFORM_HTONS(x)
#define lwip_htonl(x) LWIP_PLATFORM_HTONL(x)
#define lwip_ntohl(x) LWIP_PLATFORM_HTONL(x)

#define htons(x) lwip_htons(x)
#define ntohs(x) lwip_ntohs(x)
#define htonl(x) lwip_htonl(x)
#define ntohl(x) lwip_ntohl(x)





#endif
