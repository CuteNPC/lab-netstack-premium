#ifndef _UTILS_DEBUG_H_
#define _UTILS_DEBUG_H_
#include <stdio.h>

#define DEBUGx
#define DEBUG2x

#ifdef DEBUG
#define debugPrint(...)      \
    {                        \
        printf("[DEBUG]: "); \
        printf(__VA_ARGS__); \
        printf("\n");        \
    }
#else
#define debugPrint(...)
#endif // DEBUG

#ifdef DEBUG2
#define debugPrint2(...)      \
    {                         \
        printf("[DEBUG2]: "); \
        printf(__VA_ARGS__);  \
        printf("\n");         \
    }
#else
#define debugPrint2(...)
#endif // DEBUG

int debugPrintState(int num, int);

#endif