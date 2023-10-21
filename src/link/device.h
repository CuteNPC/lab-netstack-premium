/**
 * @file device.h
 * @brief Support network interface management.
 *
 */

#ifndef _LINK_DEVICE_H_
#define _LINK_DEVICE_H_

#include <pcap.h>
#include "link/ethheader.h"

struct Device
{
    char *deviceName;
    pcap_t *handle;
    struct Device *nextPointer;
    int32_t deviceDescriptor;
    struct MacAddr macAddr;
};

extern struct Device *deviceListHead;
extern struct Device *deviceListTail;
extern int deviceDescriptorCount;

/**
 * @brief Add a device to the library for sending/receiving packets.
 *
 * @param deviceName Name of network device to send/receive frame on.
 * @return A non -negative _device -ID_ on success , -1 on error.
 */
int addDevice(const char *deviceName);

/**
 * @brief Find a device added by ‘addDevice ‘.
 *
 * @param deviceName Name of network device to send/receive frame on.
 * @return A non -negative _device -ID_ on success , -1 on error.
 */
int findDevice(const char *deviceName);

/**
 * @brief Get the MAC address of a device.
 *
 * @param deviceDescriptor Name of network device to send/receive frame on.
 * @return A non -negative _device -ID_ on success , -1 on error.
 */
uint8_t *getMacAddr(int deviceDescriptor);

/**
 * @brief Find a device added by ‘addDevice ‘.
 *
 * @param deviceName Name of network device to send/receive frame on.
 * @return A device pointer , NULL on error.
 */
struct Device *findDeviceByDescriptorRetPtr(int deviceDescriptor);

#endif