#include "network/arp.h"
#include "link/link.h"
#include <stdlib.h>
#include <pthread.h>
#include "utils/time.h"

struct ARPList arpList;
pthread_mutex_t arpMutex;

struct ARPPacket createARPPacket(uint16_t code,
                                 struct MacAddr srcHAddr,
                                 IPAddr srcPAddr,
                                 struct MacAddr dstHAddr,
                                 IPAddr dstPAddr)
{
    struct ARPPacket arpPacket;
    arpPacket.hType = htons(0x0001);
    arpPacket.pType = htons(0x0800);
    arpPacket.hLen = 6;
    arpPacket.pLen = 4;
    arpPacket.code = htons(code);
    arpPacket.srcHAddr = srcHAddr;
    arpPacket.srcPAddr = srcPAddr;
    if (code == 1)
        memset(&arpPacket.dstHAddr, 0, sizeof(arpPacket.dstHAddr));
    else if (code == 2)
        arpPacket.dstHAddr = dstHAddr;
    arpPacket.dstPAddr = dstPAddr;
    return arpPacket;
}

int sendARPPacket(uint16_t code,
                  struct MacAddr srcHAddr,
                  IPAddr srcPAddr,
                  struct MacAddr dstHAddr,
                  IPAddr dstPAddr,
                  struct Device *device)
{
    struct ARPPacket arpPacket = createARPPacket(code,
                                                 srcHAddr, srcPAddr,
                                                 dstHAddr, dstPAddr);
    struct MacAddr destMac;
    if (code == 1)
        destMac = BROAD_MAC;
    else if (code == 2)
        destMac = dstHAddr;

    if (sendFrame(&arpPacket, sizeof(struct ARPPacket), ETHTYPE_ARP, destMac, device))
    {
        fprintf(stderr, "Fail to send!\n");
        return -1;
    }
    return 0;
}

int sendARPRequest(IPAddr dstPAddr)
{
    uint16_t code = 1;

    for (struct Device *device = deviceList.head->nextPointer;
         device != NULL;
         device = device->nextPointer)
    {
        struct MacAddr srcHAddr = device->macAddr;
        IPAddr srcPAddr = device->ipAddr;
        struct ARPPacket arpPacket = createARPPacket(code,
                                                     srcHAddr, srcPAddr,
                                                     ZREO_MAC, dstPAddr);
        if (sendFrame(&arpPacket, sizeof(struct ARPPacket), ETHTYPE_ARP, BROAD_MAC, device))
        {
            fprintf(stderr, "Fail to send!\n");
            return -1;
        }
    }
    return 0;
}

int sendARPReply(struct MacAddr dstHAddr, IPAddr dstPAddr, struct Device *device)
{
    uint16_t code = 2;

    struct ARPPacket arpPacket = createARPPacket(code,
                                                 device->macAddr, device->ipAddr,
                                                 dstHAddr, dstPAddr);
    if (sendFrame(&arpPacket, sizeof(struct ARPPacket), ETHTYPE_ARP, dstHAddr, device))
    {
        fprintf(stderr, "Fail to send!\n");
        return -1;
    }
    return 0;
}

void insertARPNode(struct ARPNode arpNode)
{
    pthread_mutex_lock(&arpMutex);
    for (struct ARPNode *p = arpList.head->nextPointer;
         p != NULL;
         p = p->nextPointer)
        if (p->ipAddr == arpNode.ipAddr)
        {
            p->macAddr = arpNode.macAddr;
            p->delTime = arpNode.delTime;
            pthread_mutex_unlock(&arpMutex);
            return;
        }
    struct ARPNode *arpNodePtr = (struct ARPNode *)malloc(sizeof(struct ARPNode));
    *arpNodePtr = arpNode;
    arpList.tail->nextPointer = arpNodePtr;
    arpList.tail = arpNodePtr;
    pthread_mutex_unlock(&arpMutex);
}

void handleARPPacket(const uint8_t *packet, uint32_t pktlen, struct EthHeader ethHdr, struct Device *device)
{
    struct ARPPacket arpPacket = *(struct ARPPacket *)packet;

    if (!(arpPacket.hType == htons(0x0001) &&
          arpPacket.pType == htons(0x0800) &&
          arpPacket.hLen == 6 &&
          arpPacket.pLen == 4))
        return;

    if (arpPacket.code == 1)
    {
        for (struct Device *device = deviceList.head->nextPointer;
             device != NULL;
             device = device->nextPointer)
            if (arpPacket.dstPAddr == device->ipAddr)
                sendARPReply(arpPacket.srcHAddr, arpPacket.srcPAddr, device);
    }
    else if (arpPacket.code == 2)
    {
        struct ARPNode arpNode;
        arpNode.ipAddr = arpPacket.srcPAddr;
        arpNode.macAddr = arpPacket.srcHAddr;
        arpNode.delTime = getSecondTime() + 60;
        arpNode.nextPointer = NULL;
        insertARPNode(arpNode);
    }
}

int initArpList()
{
    static int _initialized = 0;
    if (_initialized == 1)
        return 1;
    arpList.head = (struct ARPNode *)malloc(sizeof(struct ARPNode));
    if (arpList.head == NULL)
    {
        fprintf(stderr, "Fail to initialize ARP list!\n");
        return -1;
    }
    _initialized = 1;
    arpList.head->nextPointer = NULL;
    arpList.tail = arpList.head;
    pthread_mutex_init(&arpMutex, NULL);
    return 0;
}

int queryARPList(IPAddr ipAddr, struct MacAddr *macAddr)
{
    pthread_mutex_lock(&arpMutex);

    for (struct ARPNode *node = arpList.head->nextPointer;
         node != NULL;
         node = node->nextPointer)
        if (node->ipAddr == ipAddr)
        {
            pthread_mutex_unlock(&arpMutex);
            return 0;
            *macAddr = node->macAddr;
        }
    
    pthread_mutex_unlock(&arpMutex);
    return -1;
}

