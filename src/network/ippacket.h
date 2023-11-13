/**
 * @file ippacket.h
 * @brief Library supporting sending/receiving IP packets encapsulated
 * in an Ethernet II frame.
 */

#ifndef _NETWORK_IPPACKET_H_
#define _NETWORK_IPPACKET_H_

#include <netinet/ip.h>
#include "link/device.h"
#include "network/ipheader.h"

struct ResendIPTask
{
    uint32_t src;
    uint32_t dest;
    int proto;
    const void *buf;
    int len;
    struct ResendIPTask *nextPointer;
};

struct ResendIPTaskList
{
    struct ResendIPTask *head;
    struct ResendIPTask *tail;
    int count;
};

extern struct ResendIPTaskList resendIPTaskList;


typedef int (*IPPacketReceiveCallback)(const void *buf, uint32_t len, struct IpHeader, struct Device *);

extern IPPacketReceiveCallback networkCallBack[64];
extern int networkCallBackCnt;
int initResendIPTaskList();

void processResendIPTask();

int sendIPPacket(const uint32_t src, const uint32_t dest,
                 int proto, const void *buf, int len, int noRetry);


int setIPPacketReceiveCallback(IPPacketReceiveCallback callback);


int handleIPPacket(const void *packet, uint32_t pktlen, struct EthHeader ethHdr, struct Device *device);

#endif