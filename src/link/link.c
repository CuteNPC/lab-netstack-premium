/**
 * @file link.c
 * @brief Support network interface management, as well as sending and receiving Ethernet frames.
 */
#include <stdio.h>
#include <stdlib.h>
#include "link/link.h"

int initLinkLayer()
{
    static int _initialized = 0;
    if (_initialized == 1)
    {
        fprintf(stderr, "Network device management system has been initialized!\n");
        return 1;
    }

    deviceListHead = (struct Device *)malloc(sizeof(struct Device));
    if (deviceListHead == NULL)
    {
        fprintf(stderr, "Fail to initialize network device management system!\n");
        return -1;
    }
    _initialized = 1;
    deviceDescriptorCount = 0;
    deviceListHead->nextPointer = NULL;
    deviceListTail = deviceListHead;
    linkCallback = NULL;
    return 0;
}