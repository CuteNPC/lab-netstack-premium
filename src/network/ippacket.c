/**
 * @file ippacket.c
 * @brief Library supporting sending/receiving IP packets encapsulated
 * in an Ethernet II frame.
 */

#include <stdio.h>
#include "link/device.h"
#include "network/ippacket.h"
#include "network/ipheader.h"
#include "network/route.h"
#include "netinet/in.h"
#include "link/frame.h"
#include "utils/time.h"
#include "network/arp.h"
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include "utils/debug.h"

uint32_t random_drop = 0;

struct CallbackList networkCallbackList;

struct ResendIPTaskList resendIPTaskList;
pthread_mutex_t resendTaskListMutex;

int initResendIPTaskList()
{
    resendIPTaskList.head = (struct ResendIPTask *)malloc(sizeof(struct ResendIPTask));
    resendIPTaskList.count = 0;
    resendIPTaskList.head->nextPointer = NULL;
    resendIPTaskList.tail = resendIPTaskList.head;
    pthread_mutex_init(&resendTaskListMutex, NULL);
    return 0;
}

int setResendIPTask(const uint32_t src, const uint32_t dest,
                    int proto, const void *buf, int len)
{
    struct ResendIPTask *task = (struct ResendIPTask *)malloc(sizeof(struct ResendIPTask));

    task->buf = buf;
    task->dest = dest;
    task->len = len;
    task->proto = proto;
    task->src = src;
    task->nextPointer = NULL;

    pthread_mutex_lock(&resendTaskListMutex);
    resendIPTaskList.tail->nextPointer = task;
    resendIPTaskList.tail = task;
    pthread_mutex_unlock(&resendTaskListMutex);
}

int sendIPPacket(const uint32_t src, const uint32_t dest,
                 int proto, const void *buf, int len, int noRetry)
{
    static int id = 0;
    uint8_t packet[0x10000];
    if (len > 0x10000 - sizeof(struct IpHeader) || len <= 0)
    {
        fprintf(stderr, "Data length error!\n");
        return -1;
    }
    *(struct IpHeader *)packet = createIpHeader(
        IPVERSION_IPv4, htons(len + 20),
        htons(id++), 0, 0, 0,
        255, proto,
        src, dest);
    memcpy(packet + sizeof(struct IpHeader), buf, len);
    if (dest == BROAD_IP)
    {
        for (struct Device *device = deviceList.head->nextPointer;
             device != NULL;
             device = device->nextPointer)
            sendFrame(packet, len + sizeof(struct IpHeader), ETHTYPE_IPv4, BROAD_MAC, device);
        return 0;
    }
    struct MacAddr dstmac;
    struct Device *device;
    if (queryRouteTable(dest, &dstmac, &device))
    {
        if (queryARPList(dest, &dstmac))
        {
            // debugPrint3("IP Send: %s ",device->deviceName);
            // printMacAddr(dstmac);
            // debugPrint3("\n");
            sendFrame(packet, len + sizeof(struct IpHeader), ETHTYPE_IPv4, dstmac, device);
        }
        else if (!noRetry)
            setResendIPTask(src, dest, proto, buf, len);
        else
            return -1;
    }
    else
        return -1;
    return 0;
}

void processResendIPTask()
{
    struct ResendIPTask *task;
    int run = 0;
    pthread_mutex_lock(&resendTaskListMutex);

    if (resendIPTaskList.head->nextPointer)
    {
        task = resendIPTaskList.head->nextPointer;
        resendIPTaskList.head->nextPointer = task->nextPointer;
        if (task == resendIPTaskList.tail)
            resendIPTaskList.tail = resendIPTaskList.head;
        run = 1;
    }

    pthread_mutex_unlock(&resendTaskListMutex);
    if (run)
        sendIPPacket(task->src, task->dest, task->proto, task->buf, task->len, 1);
}

int setIPPacketReceiveCallback(IPPacketReceiveCallback callback)
{
    static int _initialized = 0;
    if (_initialized == 0)
    {
        initCallbackList(&networkCallbackList);
        _initialized = 1;
    }
    insertCallback(&networkCallbackList, (void *)callback);
}

int isMyPacket(struct IpHeader ipHeader)
{
    if (ipHeader.dst == BROAD_IP)
        return 1;
    for (struct Device *device = deviceList.head->nextPointer;
         device != NULL;
         device = device->nextPointer)
        if (ipHeader.dst == device->ipAddr)
            return 1;
    return 0;
}

int deliverIPPacket(const uint8_t *packet, uint32_t pktlen, IPAddr ip)
{
    // fflush(stdout);
    struct MacAddr dstmac;
    struct Device *device;
    if (queryRouteTable(ip, &dstmac, &device))
    {
        // fflush(stdout);
        sendFrame(packet, pktlen, ETHTYPE_IPv4, dstmac, device);
        // debugPrint3("ipdeliver: %s",device->deviceName);
    }
    else
        return -1;
    return 0;
}

int randomDrop(const void *data, struct IpHeader ipHdr)
{
    if (((char *)data)[13] == 0 && ipHdr.protocol == IPPROTO_TCP)
    {
        uint32_t rands = rand() % 100;
        // printf("%d\n",rands);
        if (rands < random_drop)
        {
            printf("Packet Loss Happens!\n");
            // printf("loss!");
            // fflush(stdout);
            return 1;
        }
    }
    return 0;
}

/**
 * @brief "Receive Ethernet frames from a device.
 *
 * @param deviceDescriptor ID of the device(returned by ‘addDevice ‘) to receive from.
 * @param cnt The number of Ethernet frames to receive.
 * @return 0 on success , -1 on error.
 */
int handleIPPacket(const void *packet, uint32_t pktlen, struct EthHeader ethHdr, struct Device *device)
{
    if (ethHdr.type != ETHTYPE_IPv4)
        return -1;
    /* Extract the frame header and data separately */
    struct IpHeader hdr = *(struct IpHeader *)packet;
    const uint8_t *data = packet + sizeof(struct IpHeader);
    uint32_t len = pktlen - sizeof(struct IpHeader);
    if(hdr.protocol == 6)
        debugPrint3("A tcp Packet!");
    if (randomDrop(data, hdr))
        return 0;

    if (!isMyPacket(hdr))
    {
        deliverIPPacket(packet, pktlen, hdr.dst);
        return 0;
    }
    if (!networkCallbackList.head)
        return -1;

    for (struct CallbackNode *p = networkCallbackList.head->next;
         p != NULL;
         p = p->next)
        ((IPPacketReceiveCallback)p->funcPtr)(data, len, hdr, device);
    return 0;
}

void setRandomDropRate(uint32_t num)
{
    random_drop = num;
    if (random_drop > 100)
        random_drop = 100;
    printf("Set Loss Rate: %u%%\n",random_drop);
}