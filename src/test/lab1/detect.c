/* Detect devices on the network. */

#include <string.h>
#include <unistd.h>

#include "link/link.h"

void remove_newline(char* s)
{
    char* p = s + strlen(s) - 1;
    if (*p == '\n')
        *p = 0;
}

int main()
{
    char deviceName[0x100];
    initLinkLayer();
    while (1)
    {
        if (!fgets(deviceName, sizeof(deviceName), stdin))
            break;
        remove_newline(deviceName);
        if (strlen(deviceName) == 0)
            break;
        printf("Try add %s\n", deviceName);
        int device_id = addDevice(deviceName);
        if (device_id == -1)
        {
            printf("Can't add device %s!\n", deviceName);
            printf("\n");
        }
        else
        {
            printf("Add device %s, ID: %d\n", deviceName, device_id);
            printf("\n");
        }
    }

    while (1)
    {
        if (!fgets(deviceName, sizeof(deviceName), stdin))
            break;
        remove_newline(deviceName);
        if (strlen(deviceName) == 0)
            break;
        printf("Try find %s\n", deviceName);
        int device_id = findDevice(deviceName);
        if (device_id == -1)
        {
            printf("Can't find device %s!\n", deviceName);
            printf("\n");
        }
        else
        {
            printf("find device %s, ID: %d\n", deviceName, device_id);
            printf("\n");
        }
    }

    return 0;
}