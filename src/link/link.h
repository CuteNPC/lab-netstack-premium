/**
 * @file link.h
 * @brief Support network interface management, as well as sending
 * and receiving Ethernet frames.
 *
 */

#ifndef _LINK_LINK_H_
#define _LINK_LINK_H_

#include <pcap.h>
#include "link/device.h"
#include "link/frame.h"
#include "link/ethheader.h"

/**
 * @brief Initialize device management.
 *
 * @return 0 for success, -1 for failure, and 1 if it has already been initialized before.
 */
int initLinkLayer(int addAllDev);

#endif