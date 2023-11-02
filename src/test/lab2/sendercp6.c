
#include <string.h>
#include <unistd.h>
#include "pthread.h"
#include "network/network.h"

union Union
{
    IPAddr ip;
    uint8_t num[4];
};

void *run(void *p)
{
    loopCycle();
    return NULL;
}

int getIp(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    union Union u;
    u.num[0] = a;
    u.num[1] = b;
    u.num[2] = c;
    u.num[3] = d;
    return u.ip;
}

int main()
{
    initNetworkLayer();
    // double tm = getSecondTime();
    // int breaks = 1;
    // while (1)
    // {
        // for (struct Device *device = deviceList.head->nextPointer;
            //  device != NULL;
            //  device = device->nextPointer)
            // receiveFrame(device, 1);
        // processTask();
        // if (breaks == 1 && getSecondTime() - tm >= 5)
            // break;
    // }
    // fflush(stdout);
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, run, NULL);
    sleep(5);
    IPAddr udst = getIp(1, 3, 5, 7);
    IPAddr usrc = getIp(2, 4, 6, 8);

    struct Device *device;
    struct MacAddr macAddr;

    int ress;

    ress = queryRouteTable(getIp(10, 100, 3, 2), &macAddr, &device);
    setRoutingTable(udst & 0x00ffffff, 0x00ffffff, macAddr, device->deviceName);

    ress = queryRouteTable(getIp(10, 100, 2, 2), &macAddr, &device);
    setRoutingTable(udst & 0x0000ffff, 0x0000ffff, macAddr, device->deviceName);

    char data[0x1000] = "This is the CP6 test by 2100013124@stu.pku.edu.cn\n";
    size_t data_len = strlen(data);
    data[data_len - 1] = 0;
    int res = sendIPPacket(usrc, udst, 213, data, data_len, 0);

    if (res == 0)
        printf("Succeed to send: %s\n", data);
    fflush(stdout);
    return 0;
}