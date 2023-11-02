
#include <string.h>
#include <unistd.h>

#include "network/network.h"

const uint8_t desaddr[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

union Union
{
    IPAddr ip;
    uint8_t num[4];
};

int main()
{
    char data[0x1000] = "This is a test by 2100013124@stu.pku.edu.cn\n";
    initNetworkLayer();

    union Union udst;
    udst.num[0] = 1;
    udst.num[1] = 3;
    udst.num[2] = 5;
    udst.num[3] = 7;

    union Union usrc;
    usrc.num[0] = 2;
    usrc.num[1] = 4;
    usrc.num[2] = 6;
    usrc.num[3] = 8;

    struct MacAddr mac;
    mac.a[0] = 1;
    mac.a[1] = 2;
    mac.a[2] = 3;
    mac.a[3] = 4;
    mac.a[4] = 5;
    mac.a[5] = 6;

    setRoutingTable(udst.ip, UINT32_MAX, mac, "eth0");
    size_t data_len = strlen(data);
    data[data_len - 1] = 0;
    int res = sendIPPacket(usrc.ip, udst.ip, 213, data, data_len, 0);
    if (res == 0)
        printf("Succeed to send: %s\n", data);
    return 0;
}