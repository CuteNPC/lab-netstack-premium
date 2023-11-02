/**
 * @file network.h
 * @brief Support network interface management, as well as sending
 * and receiving IP packet.
 *
 */

#ifndef _NETWORK_NETWORK_H_
#define _NETWORK_NETWORK_H_

#include "network/ippacket.h"
#include "network/ipheader.h"
#include "network/arp.h"
#include "network/route.h"
#include "link/link.h"
#include "utils/time.h"


int initNetworkLayer();

#endif