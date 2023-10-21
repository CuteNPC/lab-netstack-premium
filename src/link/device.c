/**
 * @file device.c
 * @brief Support network interface management.
 */

#include <malloc.h>
#include <string.h>
#include <pcap.h>
#include <assert.h>
#include <stdlib.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>

#include "link/ethheader.h"
#include "link/device.h"

struct Device *deviceListHead;
struct Device *deviceListTail;
int deviceDescriptorCount;

struct Device *addDeviceRetPtr(const char *deviceName);
struct Device *findDeviceRetPtr(const char *deviceName);

int searchAvaiDevice(const char *name, struct MacAddr *addr)
{

    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *alldevs;

    if (pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        fprintf(stderr, "Fail at pcap_findalldevs, %s!\n", errbuf);
        return -1;
    }

    for (pcap_if_t *p_dev = alldevs; p_dev; p_dev = p_dev->next)
    {
        if (strcmp(name, p_dev->name))
            continue;
        for (pcap_addr_t *p_addr = p_dev->addresses; p_addr; p_addr = p_addr->next)
        {
            struct sockaddr *s_addr = p_addr->addr;
            if ((!s_addr) || s_addr->sa_family != AF_PACKET)
                /* Ensure that the processed network interfaces
                are based on the link-layer interface */
                continue;
            struct sockaddr_ll *sl_addr = (struct sockaddr_ll *)s_addr;
            if (sl_addr->sll_hatype != ARPHRD_ETHER)
                /* Ensure that the processed network interfaces
                 are Ethernet interfaces */
                continue;
            *addr = *(struct MacAddr *)sl_addr->sll_addr;
            pcap_freealldevs(alldevs);
            return 0;
        }
    }
    pcap_freealldevs(alldevs);
    fprintf(stderr, "Fail to find the device!\n");
    return -1;
}

int addDevice(const char *deviceName)
{
    struct Device *device = addDeviceRetPtr(deviceName);
    return device ? device->deviceDescriptor : -1;
}

struct Device *addDeviceRetPtr(const char *deviceName)
{
    char errBuf[PCAP_ERRBUF_SIZE];
    struct MacAddr macAddr;
    if (strlen(deviceName) >= 0x100)
    {
        fprintf(stderr, "Device name is too long! (No more than 256 character.)\n");
        return NULL;
    }
    /* Search the device */
    if (searchAvaiDevice(deviceName, &macAddr) == -1)
        return NULL;

    /* Open the device */
    pcap_t *handle = pcap_create(deviceName, errBuf);
    if (!handle)
    {
        fprintf(stderr, "Fail to open the device. %s\n", errBuf);
        return NULL;
    }
    /* Configure the device */
    if (pcap_setnonblock(handle, 1, errBuf) ||
        pcap_set_immediate_mode(handle, 1) ||
        pcap_set_promisc(handle, 0) ||
        pcap_activate(handle) ||
        pcap_setdirection(handle, PCAP_D_IN))
    {
        pcap_close(handle);
        fprintf(stderr, "Fail to activate the device.");
        return NULL;
    }

    struct Device *device = (struct Device *)malloc(sizeof(struct Device));

    if (device != NULL)
    {
        device->deviceName = (char *)malloc(strlen(deviceName) + 1);
        if (device->deviceName == NULL)
            free(device);
    }

    if (device == NULL || device->deviceName == NULL)
    {
        pcap_close(handle);
        fprintf(stderr, "Fail at malloc!\n");
        return NULL;
    }
    device->deviceDescriptor = ++deviceDescriptorCount;
    strcpy(device->deviceName, deviceName);
    device->nextPointer = NULL;
    device->handle = handle;
    device->macAddr = macAddr;
    /* Set up a linked list. */
    deviceListTail->nextPointer = device;
    deviceListTail = device;
    return device;
}

int findDevice(const char *deviceName)
{
    struct Device *device = findDeviceRetPtr(deviceName);
    return device ? device->deviceDescriptor : -1;
}

struct Device *findDeviceRetPtr(const char *deviceName)
{
    for (struct Device *device = deviceListHead->nextPointer;
         device != NULL;
         device = device->nextPointer)
        if (!strcmp(device->deviceName, deviceName))
            return device;
    return NULL;
}

struct Device *findDeviceByDescriptorRetPtr(int deviceDescriptor)
{
    for (struct Device *device = deviceListHead->nextPointer;
         device != NULL;
         device = device->nextPointer)
        if (device->deviceDescriptor == deviceDescriptor)
            return device;
    return NULL;
}

uint8_t *getMacAddr(int deviceDescriptor)
{
    return findDeviceByDescriptorRetPtr(deviceDescriptor)->macAddr.a;
}
