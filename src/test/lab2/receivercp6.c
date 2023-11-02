/* Send frames to the device */

#include <string.h>
#include <unistd.h>

#include "network/network.h"
int printIP(const void *buf, int len, struct IpHeader ipHeader, struct Device *device)
{
    if (ipHeader.protocol == 213)
    {
        printf("Receive: %s\n", (char *)buf);
        fflush(stdout);
    }
}
int main()
{
    initNetworkLayer();
    setIPPacketReceiveCallback(printIP);
    loopCycle();
}