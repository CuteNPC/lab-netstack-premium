/* Receive frames from the device */

#include <string.h>
#include <unistd.h>

#include <stdlib.h>

#include "link/link.h"

int print_dataload_as_string(const void *buf, uint32_t len, int id, struct EthHeader hdr)
{
    static int cnt = 0;
    char *tmp = (char *)malloc(len + 1);
    memcpy(tmp, buf, len);
    tmp[len] = 0;
    printf("Frame #%05d\n", ++cnt);
    printf("Data length: %d\n", len);
    printf("Device ID: %d\n", id);
    {
        printf("Des Mac:");
        printMacAddr(&hdr.desAddr);
        printf("\n");
    }
    {
        printf("Src Mac:");
        printMacAddr(&hdr.srcAddr);
        printf("\n");
    }
    printf("Type: 0x%04x \n", hdr.type);
    printf("String: %s\n", tmp);
    free(tmp);
    printf("Data: ");
    for (int i = 0; i < len; i++)
    {
        if (i % 4 == 0)
            printf("  ");
        if (i % 16 == 0)
            printf("\n");
        printf("%02x ", *(uint8_t *)(buf + i));
    }
    printf("\n\n\n");
    fflush(stdout);
}

int main()
{
    uint8_t desaddr[6] = {1, 2, 3, 4, 5, 6};
    char deviceName[0x100];
    uint8_t buf[0x300];
    initLinkLayer();
    if (!fgets(deviceName, sizeof(deviceName), stdin))
        return -1;

    deviceName[strlen(deviceName) - 1] = 0;
    int device_id = addDevice(deviceName);
    printf("\n");
    if (device_id == -1)
    {
        printf("Fail to add the device!\n");
        return -1;
    }
    else
    {
        printf("Succeed to add device, ID: %d\n", device_id);
    }
    setFrameReceiveCallback(print_dataload_as_string);
    receiveFrame(device_id, -1);
    return 0;
}