
#include "utils/time.h"
#include "network/route.h"
#include "network/ippacket.h"

void (*loopTaskCallback)(void) = NULL;

double getSecondTime()
{
    struct timespec tm;
    clock_gettime(CLOCK_MONOTONIC, &tm);
    double seconds = tm.tv_sec + tm.tv_nsec / 1e9;
    return seconds;
}

int setLoopTask(void (*callback)(void))
{
    loopTaskCallback = callback;
}

int processTask()
{
    broadcastRouteTable();
    updateRouteTable();
    processResendIPTask();
    if (loopTaskCallback)
        loopTaskCallback();
}