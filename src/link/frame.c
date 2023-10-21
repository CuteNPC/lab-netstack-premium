/**
 * @file frame.c
 * @brief Support sending and receiving Ethernet frames.
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

#include "link/frame.h"
#include "link/device.h"
#include "link/ethheader.h"

frameReceiveCallback linkCallback;

int sendFrame(const void *buf, int len, int ethType, const void *destMac, int deviceDescriptor)
{
    uint8_t frame[0x10000];
    if (len > 0x10000 - sizeof(struct EthHeader) || len <= 0)
    {
        fprintf(stderr, "Data length error!\n");
        return -1;
    }

    struct Device *device = findDeviceByDescriptorRetPtr(deviceDescriptor);
    if (device == NULL)
    {
        fprintf(stderr, "Device not found!\n");
        return -1;
    }

    /* Set up the header. */
    *(struct EthHeader *)frame = createEthHeader(
        *(struct MacAddr *)destMac,
        device->macAddr,
        htons((uint16_t)ethType));

    /* Set up the dataload. */
    memcpy(frame + sizeof(struct EthHeader), buf, len);

    /* Send. */
    if (pcap_sendpacket(device->handle, frame, len + sizeof(struct EthHeader)))
    {
        fprintf(stderr, "Fail to send! %s\n", pcap_geterr(device->handle));
        return -1;
    }
    return 0;
}

void handleFrame(uint8_t *user_data, const struct pcap_pkthdr *pkthdr, const uint8_t *pktdata)
{
    /* Extract the frame header and data separately */
    int deviceDescriptor = (int)(uint64_t)user_data;
    struct EthHeader hdr = *(struct EthHeader *)pktdata;
    hdr.type = ntohs(hdr.type);
    const uint8_t *data = pktdata + sizeof(struct EthHeader);
    uint32_t len = pkthdr->len - sizeof(struct EthHeader);
    /* Callback */
    if (linkCallback)
        linkCallback(data, len, deviceDescriptor, hdr);
}

int receiveFrame(int deviceDescriptor, int cnt)
{
    struct Device *device = findDeviceByDescriptorRetPtr(deviceDescriptor);
    if (device == NULL)
    {
        fprintf(stderr, "Device not found!\n");
        return -1;
    }
    u_char *deviceDescriptor_p = (u_char *)(long)deviceDescriptor;
    int loop_forever = (cnt < 0);
    while (loop_forever || cnt > 0)
    {
        pcap_dispatch(device->handle, 1, handleFrame, deviceDescriptor_p);
        cnt--;
    }

    return 0;
}

int setFrameReceiveCallback(frameReceiveCallback callback)
{
    linkCallback = callback;
    return 0;
}
