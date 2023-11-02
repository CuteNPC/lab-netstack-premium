/**
 * @file network.c
 * @brief Support network interface management, as well as sending
 * and receiving IP packet.
 *
 */

#include "network/ippacket.h"
#include "network/ipheader.h"
#include "network/arp.h"
#include "network/route.h"
#include "network/network.h"
#include "link/link.h"

int initNetworkLayer()
{
    initLinkLayer(1);
    initArpList();
    initRouteTable();
    initResendIPTaskList();
}