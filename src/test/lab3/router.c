#include "unp.h"
#include <time.h>
#include <unistd.h>
#include "network/ippacket.h"
#include "transport/transport.h"
#include "utils/time.h"

int main(int argc, char *argv[])
{
    uint32_t lossPackRate = 0;
    if (argc > 1)
        if (sscanf(argv[1], "%u", &lossPackRate) == 1)
            setRandomDropRate(lossPackRate);
    initTransportLayer();

    double startTime = getSecondTime();
    double endTime = getSecondTime();
    while (endTime < startTime + 120)
    {
        sleep(10);
        endTime = getSecondTime();
    }
}
