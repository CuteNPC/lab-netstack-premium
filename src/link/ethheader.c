/**
 * @file ethheader.c
 * @brief Define the header of an Ethernet frame and some related methods.
 */

#include <stdio.h>

#include "link/ethheader.h"

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

void printMacAddr(void *desAddr)
{
    uint8_t *p = (void *)desAddr;
    printf("%02x:%02x:%02x:%02x:%02x:%02x",
           p[0], p[1], p[2], p[3], p[4], p[5]);
}