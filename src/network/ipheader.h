#ifndef _NETWORK_IPHEADER_H_
#define _NETWORK_IPHEADER_H_

#include <sys/types.h>
#include <stdint.h>

#define IPVERSION_IPv4 4
#define IPVERSION_IPv6 6
#define IP_ADDR_LEN 8

extern uint32_t BROAD_IP;

typedef uint32_t IPAddr;

struct IPv6Addr
{
    uint32_t a[4];
};

struct IpHeader
{
    uint8_t verHdrlen;
    uint8_t service;
    uint16_t len;

    uint16_t id;
    uint16_t flagOffset;

    uint8_t ttl;
    uint8_t protocol;
    uint16_t check;

    uint32_t src;
    uint32_t dst;
};// __attribute__((__packed__));

struct IpHeader createIpHeader(
    uint8_t version,
    uint16_t len,

    uint16_t id,
    uint8_t flagMF,
    uint8_t flagDF,
    uint16_t Offset,

    uint8_t ttl,
    uint8_t protocol,

    uint32_t src,
    uint32_t dst);

#endif