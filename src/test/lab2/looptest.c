#include "network/network.h"
#include <stdio.h>

int main()
{
    initNetworkLayer();
    setLoopTask(printRoute);
    loopCycle();
}