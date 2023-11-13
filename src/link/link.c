/**
 * @file link.c
 * @brief Support network interface management, as well as sending and receiving Ethernet frames.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "link/link.h"

int initLinkLayer(int addAllDev)
{
    static int _initialized = 0;
    if (_initialized == 1)
        return 1;
    
    memset((void *)&BROAD_MAC, 0xff, sizeof(BROAD_MAC));

    deviceList.head = (struct Device *)malloc(sizeof(struct Device));
    if (deviceList.head == NULL)
    {
        fprintf(stderr, "Fail to initialize network device management system!\n");
        return -1;
    }
    _initialized = 1;
    deviceList.maxcount = 0;
    deviceList.head->nextPointer = NULL;
    deviceList.tail = deviceList.head;
    if(addAllDev)
        addAllDevice();
    return 0;
}