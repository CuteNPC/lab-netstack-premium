#include "transport/mainthread.h"
#include "network/network.h"
#include "transport/loopthread.h"
#include "network/route.h"
#include "utils/debug.h"
#include <stdlib.h>
#include <pthread.h>

void *runLabStack(void *p)
{
    loopCycle();
    return NULL;
}

int initTransportLayer()
{
    static int _initialized = 0;
    if (_initialized == 1)
        return 1;
    _initialized = 1;
    initSocketList();
    initTCPHandleFunList();
    initNetworkLayer();
    setIPPacketReceiveCallback(handleTCPPacket);
    setTCPPacketReceiveCallback(handleTCPMain);
    setLoopTask(asyncSendTCPPacket);
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, runLabStack, NULL);
    while (1)
        if (routeStop)
            break;
#ifdef DEBUG
    printRoute();
#endif // DEBUG
    return 0;
}