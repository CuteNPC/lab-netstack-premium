/* Send frames to the device */

#include <string.h>
#include <unistd.h>

#include "link/link.h"

const uint8_t desaddr[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

int main()
{
    char deviceName[0x100];
    char data[0x1000];

    initLinkLayer();

    printf("Input the device name: ");
    if (!fgets(deviceName, sizeof(deviceName), stdin))
        return -1;
    deviceName[strlen(deviceName) - 1] = 0;

    int device_id = addDevice(deviceName);
    printf("\n");
    if (device_id == -1)
    {
        printf("Device not found!\n");
        return -1;
    }
    else
    {
        printf("Device ID: %d\n", device_id);
    }

    printf("DesMacAddr: ");
    printMacAddr((void *)desaddr);
    printf("\n");
    printf("SrcMacAddr: ");
    printMacAddr((void *)getMacAddr(device_id));
    printf("\n\n");

    printf("Input the data: \n");
    while (fgets(data, sizeof(data), stdin))
    {
        size_t data_len = strlen(data);
        data[data_len - 1] = 0;
        int res = sendFrame(data, data_len, 0x0800, desaddr, device_id);
        if (res == 0)
            printf("Succeed to send: %s\n", data);
    }
    return 0;
}