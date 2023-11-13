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
#include <pthread.h>

#include "link/frame.h"
#include "link/device.h"
#include "link/ethheader.h"
#include "network/ippacket.h"
#include "utils/time.h"
#include "utils/debug.h"
#include "network/arp.h"
#include "utils/callbacklist.h"

struct CallbackList linkCallbackList;

int sendFrame(const void *buf, int len, int ethType, struct MacAddr destMac, struct Device *device)
{
    uint8_t frame[0x10000];
    if (len > 0x10000 - sizeof(struct EthHeader) || len <= 0)
    {
        fprintf(stderr, "Data length error!\n");
        return -1;
    }

    /* Set up the header. */
    *(struct EthHeader *)frame = createEthHeader(
        destMac,
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
    debugPrint("handleFrame");
    /* Extract the frame header and data separately */
    struct Device *device = (struct Device *)user_data;
    struct EthHeader hdr = *(struct EthHeader *)pktdata;
    hdr.type = ntohs(hdr.type);
    const uint8_t *data = pktdata + sizeof(struct EthHeader);
    uint32_t len = pkthdr->len - sizeof(struct EthHeader);
    if (!macAddrEqual(hdr.desAddr, BROAD_MAC) && !macAddrEqual(hdr.desAddr, device->macAddr))
        return;
    if (!linkCallbackList.head)
        return;
    for (struct CallbackNode *p = linkCallbackList.head->next;
         p != NULL;
         p = p->next)
        ((frameReceiveCallback)p->funcPtr)(data, len, hdr, device);
}

int receiveFrame(struct Device *device, int cnt)
{
    int loop_forever = (cnt < 0);
    while (loop_forever || cnt > 0)
    {
        pcap_dispatch(device->handle, 1, handleFrame, (u_char *)device);
        cnt--;
    }

    return 0;
}

int loopCycle()
{
    static pthread_mutex_t mutex;
    if (pthread_mutex_trylock(&mutex) == 0)
    {
        while (1)
        {
            for (struct Device *device = deviceList.head->nextPointer;
                 device != NULL;
                 device = device->nextPointer)
                receiveFrame(device, 1);
            processTask();
        }
        pthread_mutex_unlock(&mutex); // 释放锁
    }
    else
    {
        printf("Looping has launched!\n");
    }
}

void setFrameReceiveCallback(frameReceiveCallback callback)
{
    static int _initialized = 0;
    if (_initialized == 0)
    {
        initCallbackList(&linkCallbackList);
        _initialized = 1;
    }
    insertCallback(&linkCallbackList, (void *)callback);
}
