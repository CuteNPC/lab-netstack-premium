#include <stdio.h>

#include "network/ipheader.h"
#include "netinet/in.h"

uint32_t BROAD_IP = 0xffffffff;

uint16_t calcCheck(uint16_t *p)
{
    uint32_t check = 0;
    for (int i = 0; i < 10; i++)
        check += p[i];
    check = (check & 0xffff) + (check >> 16);
    check = (check & 0xffff) + (check >> 16);
    return (uint16_t)(~check);
}

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
    uint32_t dst)
{
    struct IpHeader h;
    h.verHdrlen = ((version & 0b1111) << 4) | 5; // Big endian 5
    h.service = 0;
    h.len = len;
    h.id = id;

    h.flagOffset = 0x40;

    h.ttl = ttl;
    h.protocol = protocol;
    h.check = 0;
    h.src = src;
    h.dst = dst;
    h.check = calcCheck((uint16_t *)&h);
    return h;
}