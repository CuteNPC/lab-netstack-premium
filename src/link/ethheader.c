/**
 * @file ethheader.c
 * @brief Define the header of an Ethernet frame and some related methods.
 */

#include <stdio.h>

#include "link/ethheader.h"

struct MacAddr BROAD_MAC;
struct MacAddr ZREO_MAC;

struct EthHeader createEthHeader(struct MacAddr desAddr,
                                 struct MacAddr srcAddr,
                                 uint16_t type)
{
    struct EthHeader h;
    h.desAddr = desAddr;
    h.srcAddr = srcAddr;
    h.type = type;
    return h;
}

int macAddrEqual(struct MacAddr a, struct MacAddr b)
{
    for (int i = 0; i < MAC_ADDR_LEN; i++)
        if (a.a[i] != b.a[i])
            return 0;
    return 1;
}

void printMacAddr(struct MacAddr desAddr)
{
    uint8_t *p = desAddr.a;
    printf("%02x:%02x:%02x:%02x:%02x:%02x",
           p[0], p[1], p[2], p[3], p[4], p[5]);
}