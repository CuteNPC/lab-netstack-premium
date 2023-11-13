#ifndef _UTILS_TIME_H_
#define _UTILS_TIME_H_

#include <time.h>

double getSecondTime();

void processTask();

void setLoopTask(void (*callback)(void));

#endif