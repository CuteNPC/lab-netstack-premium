#ifndef _TRANSPORT_TCPHANDLER_H_
#define _TRANSPORT_TCPHANDLER_H_

#include <stdint.h>
#include "network/ipheader.h"
#include "link/ethheader.h"
#include "transport/tcpheader.h"
#include "network/ippacket.h"
#include "transport/mainthread.h"

typedef int (*TCPPacketReceiveCallback)(struct Socket *, const void *, const void *, int, struct TCPHeader, struct IpHeader);

int initTCPHandleFunList();
int handleTCPPacket(const void *packet, uint32_t pktlen, struct IpHeader ipHdr, struct Device *device);
int handleTCPMain(struct Socket *match, const void *packet, const void *data, int len, struct TCPHeader tcpHeader, struct IpHeader ipHeader);
void asyncSendTCPPacket();
int setTCPPacketReceiveCallback(TCPPacketReceiveCallback callback);

#endif
