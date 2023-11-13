/**
 * @file frame.h
 * @brief Support sending and receiving Ethernet frames.
 */

#ifndef _LINK_FRAME_H_
#define _LINK_FRAME_H_

#include <pcap.h>
#include "link/ethheader.h"
#include "link/device.h"
#include "utils/callbacklist.h"

/**
 * @brief Process the dataload upon receiving it.
 *
 * @param buf Pointer to the dataload without header.
 * @param len Length of the dataload without header.
 * @param id ID of the device (returned by ‘addDevice ‘) receiving
 * current dataload.
 * @param header Header of the Ethernet II frame.
 * @return 0 on success , -1 on error.
 *
 */
typedef int (*frameReceiveCallback)(const void *, uint32_t, struct EthHeader, struct Device *);

extern struct CallbackList linkCallbackList;

int sendFrame(const void *buf, int len, int ethtype, struct MacAddr destMac, struct Device *);

/**
 * @brief "Receive Ethernet frames from a device.
 *
 * @param deviceDescriptor ID of the device(returned by ‘addDevice ‘) to receive from.
 * @param cnt The number of Ethernet frames to receive.
 * @return 0 on success , -1 on error.
 */
int receiveFrame(struct Device *device, int cnt);

/**
 * @brief Register a callback function to be called each time an
 * Ethernet II frame was received.
 *
 * @param callback the callback function.
 * @return 0 on success , -1 on error.
 * @see frameReceiveCallback
 */
void setFrameReceiveCallback(frameReceiveCallback callback);

int loopCycle();

#endif