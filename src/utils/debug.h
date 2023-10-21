#ifndef _UTILS_DEBUG_H_
#define _UTILS_DEBUG_H_

#define debugPrint(format, ...)        \
    {                                  \
        printf(format, ##__VA_ARGS__); \
    }

#endif