#ifndef _NETWORK_ARP_H_
#define _NETWORK_ARP_H_

#include <stdint.h>
#include <string.h>
#include "link/ethheader.h"
#include "network/ipheader.h"
#include <netinet/in.h>
#include <stdio.h>
#include "link/device.h"

struct ARPNode
{
    IPAddr ipAddr;
    struct MacAddr macAddr;
    uint64_t delTime;
    struct ARPNode *nextPointer;
};

struct ARPList
{
    struct ARPNode *head;
    struct ARPNode *tail;
};

extern struct ARPList arpList;

struct ARPPacket
{
    uint16_t hType;
    uint16_t pType;
    uint8_t hLen;
    uint8_t pLen;
    uint16_t code;
    struct MacAddr srcHAddr;
    IPAddr srcPAddr;
    struct MacAddr dstHAddr;
    IPAddr dstPAddr;
} __attribute__((__packed__));

int initArpList();

struct ARPPacket createARPPacket(uint16_t code,
                                 struct MacAddr srcHAddr,
                                 IPAddr srcPAddr,
                                 struct MacAddr dstHAddr,
                                 IPAddr dstPAddr);

void handleARPPacket(const uint8_t *packet, uint32_t pktlen, struct EthHeader ethHdr, struct Device *device);

int sendARPPacket(uint16_t code,
                  struct MacAddr srcHAddr,
                  IPAddr srcPAddr,
                  struct MacAddr dstHAddr,
                  IPAddr dstPAddr,
                  struct Device *);

int queryARPList(IPAddr ipAddr, struct MacAddr *macAddr);

#endif