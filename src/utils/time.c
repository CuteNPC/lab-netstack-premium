
#include "utils/time.h"
#include "network/route.h"
#include "network/ippacket.h"
#include "utils/callbacklist.h"

struct CallbackList timerCallbackList;

double getSecondTime()
{
    struct timespec tm;
    clock_gettime(CLOCK_MONOTONIC, &tm);
    double seconds = tm.tv_sec + tm.tv_nsec / 1e9;
    return seconds;
}

void setLoopTask(void (*callback)(void))
{
    static int _initialized = 0;
    if (_initialized == 0)
    {
        initCallbackList(&timerCallbackList);
        _initialized = 1;
    }
    insertCallback(&timerCallbackList, (void *)callback);
}

void processTask()
{
    if (!timerCallbackList.head)
        return;
    for (struct CallbackNode *p = timerCallbackList.head->next;
         p != NULL;
         p = p->next)
        ((void (*)(void))p->funcPtr)();
}