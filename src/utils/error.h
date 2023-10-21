#ifndef _UTILS_ERROR_H_
#define _UTILS_ERROR_H_

#define errorExit(format, ...)                  \
    {                                           \
        fprintf(stderr, format, ##__VA_ARGS__); \
        exit(-1);                               \
    }

#endif