/**
 * @file device.h
 * @brief Support network interface management.
 *
 */

#ifndef _LINK_DEVICE_H_
#define _LINK_DEVICE_H_

#include <pcap.h>
#include "link/ethheader.h"
// #include "network/ipheader.h"

struct Device
{
    char *deviceName;
    pcap_t *handle;
    struct Device *nextPointer;
    int32_t deviceDescriptor;
    struct MacAddr macAddr;
    uint32_t ipAddr;
    // struct IPv6Addr ipv6Addr;
};

struct DeviceList
{
    struct Device *head;
    struct Device *tail;
    int maxcount;
};

extern struct DeviceList deviceList;

/**
 * @brief Add a device to the library for sending/receiving packets.
 *
 * @param deviceName Name of network device to send/receive frame on.
 * @return A non -negative _device -ID_ on success , -1 on error.
 */
struct Device *addDevice(const char *deviceName);

/**
 * @brief Find a device added by ‘addDevice ‘.
 *
 * @param deviceName Name of network device to send/receive frame on.
 * @return A non -negative _device -ID_ on success , -1 on error.
 */
struct Device *findDevice(const char *deviceName);

/**
 * @brief Get the MAC address of a device.
 *
 * @param deviceDescriptor Name of network device to send/receive frame on.
 * @return A non -negative _device -ID_ on success , -1 on error.
 */
struct MacAddr getMacAddr(struct Device *);

/**
 * @brief Find a device added by ‘addDevice ‘.
 *
 * @param deviceName Name of network device to send/receive frame on.
 * @return A device pointer , NULL on error.
 */
struct Device *findDeviceByDescriptorRetPtr(int deviceDescriptor);

/**
 * @brief Get the first Ethernet device ‘.
 *
 * @return A device pointer , NULL on error.
 */

struct Device *getFirstDevice();


int addAllDevice();

#endif