#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "network/route.h"
#include "link/link.h"
#include "utils/time.h"
#include "network/ippacket.h"
#include <pthread.h>
#include <math.h>
#include <arpa/inet.h>
#include <time.h>

const double broadInterval = 1;
const double updateInterval = 2.5;
const double printInterval = 1.5;

struct RouteEntry *manRouteTable;
uint32_t manRouteTableCnt;

struct RouteEntry *onlineRouteTable;
uint32_t onlineRouteTableCnt;

struct RouteEntry *tmpRouteTable;
uint32_t tmpRouteTableCnt;

pthread_mutex_t tableMutex;

int routeStop;



int compareRouteEntries(const void *a, const void *b);
void removeDuplicates(struct RouteEntry *entries, int *numEntries);

int addDefaultRouteTable(struct RouteEntry **theRouteTable, int *theRouteTableCnt)
{
    *theRouteTable = (struct RouteEntry *)realloc(*theRouteTable, sizeof(struct RouteEntry) * (deviceList.maxcount + *theRouteTableCnt));
    if (*theRouteTable == NULL)
    {
        fprintf(stderr, "Fail to initialize ARP list!\n");
        return -1;
    }
    struct Device *device;
    for (device = deviceList.head->nextPointer;
         device != NULL;
         device = device->nextPointer, (*theRouteTableCnt)++)
    {
        (*theRouteTable)[*theRouteTableCnt].device = device;
        (*theRouteTable)[*theRouteTableCnt].ipAddr = device->ipAddr;
        (*theRouteTable)[*theRouteTableCnt].macAddr = device->macAddr;
        (*theRouteTable)[*theRouteTableCnt].mask = BROAD_IP;
        (*theRouteTable)[*theRouteTableCnt].dist = 0;
    }
}

int initRouteTable(int enableAutoRoute)
{
    static int _initialized = 0;
    if (_initialized == 1)
        return 1;
    initLinkLayer(1);
    pthread_mutex_init(&tableMutex, NULL);
    addDefaultRouteTable(&onlineRouteTable, &onlineRouteTableCnt);
    if (enableAutoRoute)
    {
        setLoopTask(broadcastRouteTable);
        setLoopTask(updateRouteTable);
        setIPPacketReceiveCallback(handleRouteTable);
    }
    tmpRouteTable = NULL;
    tmpRouteTableCnt = 0;

    _initialized = 1;
    return 0;
}

int handleRouteTable(const void *buf, uint32_t len, struct IpHeader ipHeader, struct Device *device)
{
    if (ipHeader.protocol != 210)
        return -1;

    pthread_mutex_lock(&tableMutex);
    int newEntryCnt = (len - MAC_ADDR_LEN) / sizeof(struct RouteEntry);
    struct MacAddr macAddr = *(struct MacAddr *)(buf + len - MAC_ADDR_LEN);
    int newTmpRouteTableCnt = tmpRouteTableCnt + newEntryCnt;
    tmpRouteTable = (struct RouteEntry *)realloc(tmpRouteTable, sizeof(struct RouteEntry) * newTmpRouteTableCnt);
    memcpy(tmpRouteTable + tmpRouteTableCnt, buf, sizeof(struct RouteEntry) * newEntryCnt);
    for (int i = tmpRouteTableCnt; i < newTmpRouteTableCnt; i++)
    {
        tmpRouteTable[i].device = device;
        tmpRouteTable[i].macAddr = macAddr;
        tmpRouteTable[i].dist++;
    }
    tmpRouteTableCnt = newTmpRouteTableCnt;
    pthread_mutex_unlock(&tableMutex);
    return 1;
}

void broadcastRouteTable()
{
    static double lastBroad = -INFINITY;
    double nowTime = getSecondTime();
    if (nowTime - broadInterval < lastBroad)
        return;
    lastBroad = nowTime;

    pthread_mutex_lock(&tableMutex);
    int tableSize = sizeof(struct RouteEntry) * onlineRouteTableCnt;
    void *buf = malloc(tableSize + MAC_ADDR_LEN);
    memcpy(buf, onlineRouteTable, tableSize);

    for (struct Device *device = deviceList.head->nextPointer;
         device != NULL;
         device = device->nextPointer)
    {
        *(struct MacAddr *)(buf + tableSize) = device->macAddr;
        sendIPPacket(device->ipAddr, BROAD_IP, 210, buf, tableSize + MAC_ADDR_LEN, 0);
    }
    pthread_mutex_unlock(&tableMutex);
    return;
}


void tmpPrintRoute()
{
    printf("+tmpPrintRoute+\n");
    time_t rawtime;
    time(&rawtime);
    printf("Time: %s", asctime(localtime(&rawtime)));

    for (int i = 0; i < tmpRouteTableCnt; i++)
    {
        struct in_addr ipAddr;
        ipAddr.s_addr = tmpRouteTable[i].ipAddr;
        char ipAddressStr[32];
        inet_ntop(AF_INET, &ipAddr, ipAddressStr, sizeof(ipAddressStr));
        printf("IP: %s   dist: %02d  device: %s", ipAddressStr, tmpRouteTable[i].dist, tmpRouteTable[i].device->deviceName);
        printMacAddr(tmpRouteTable[i].macAddr);
        printf("\n");
    }
    printf("\n");
    fflush(stdout);
    pthread_mutex_unlock(&tableMutex);
    printf("-tmpPrintRoute-\n");
}


void updateRouteTable()
{
    static double lastUpdate = -INFINITY;
    double nowTime = getSecondTime();
    if (nowTime - updateInterval < lastUpdate)
        return;
    lastUpdate = nowTime;

    pthread_mutex_lock(&tableMutex);
    // if (onlineRouteTableCnt <= 1)
    // {
        // tmpPrintRoute();
        addDefaultRouteTable(&tmpRouteTable, &tmpRouteTableCnt);
        // tmpPrintRoute();
        qsort(tmpRouteTable, tmpRouteTableCnt, sizeof(struct RouteEntry), compareRouteEntries);
        // tmpPrintRoute();
        removeDuplicates(tmpRouteTable, &tmpRouteTableCnt);
        // tmpPrintRoute();

        fflush(stdout);
        struct RouteEntry *newRouteTable = (struct RouteEntry *)malloc(tmpRouteTableCnt * sizeof(struct RouteEntry));
        int newRouteTableCnt = tmpRouteTableCnt;
        memcpy(newRouteTable, tmpRouteTable, tmpRouteTableCnt * sizeof(struct RouteEntry));

        free(onlineRouteTable);
        onlineRouteTable = newRouteTable;
        onlineRouteTableCnt = newRouteTableCnt;

        free(tmpRouteTable);
        tmpRouteTable = NULL;
        tmpRouteTableCnt = 0;
    // }
    // else
    // {
        // routeStop = 1;
    // }
    pthread_mutex_unlock(&tableMutex);
    return;
}

int compareRouteEntries(const void *a, const void *b)
{
    const struct RouteEntry *entryA = (const struct RouteEntry *)a;
    const struct RouteEntry *entryB = (const struct RouteEntry *)b;

    if (entryA->mask < entryB->mask)
        return -1;
    else if (entryA->mask > entryB->mask)
        return 1;
    else if (entryA->ipAddr < entryB->ipAddr)
        return -1;
    else if (entryA->ipAddr > entryB->ipAddr)
        return 1;
    else
        return 0;
}

void removeDuplicates(struct RouteEntry *entries, int *numEntries)
{
    if (*numEntries <= 1)
        return;
    int uniqueIndex = 1;
    for (int i = 1; i < *numEntries; i++)
        if (entries[i].ipAddr != entries[uniqueIndex - 1].ipAddr || entries[i].mask != entries[uniqueIndex - 1].mask)
            entries[uniqueIndex++] = entries[i];
        else if (entries[i].dist < entries[uniqueIndex - 1].dist)
            entries[uniqueIndex - 1] = entries[i];
    *numEntries = uniqueIndex;

    int index = 0;
    for (int i = 0; i < *numEntries; i++)
        if (entries[i].dist <= 20)
            entries[index++] = entries[i];
    *numEntries = index;
}

int queryRouteTable(IPAddr ip, struct MacAddr *macAddr, struct Device **device)
{
    int success = 0;
    IPAddr bestMask = 0;

    for (int i = 0; i < manRouteTableCnt; i++)
    {
        if (manRouteTable[i].ipAddr == (ip & manRouteTable[i].mask))
            if (bestMask < manRouteTable[i].mask)
            {
                bestMask = manRouteTable[i].mask;
                *macAddr = manRouteTable[i].macAddr;
                *device = manRouteTable[i].device;
                success = 1;
            }
    }
    if (success)
        return success;

    pthread_mutex_lock(&tableMutex);
    for (int i = 0; i < onlineRouteTableCnt; i++)
        if (onlineRouteTable[i].ipAddr == (ip & onlineRouteTable[i].mask))
            if (bestMask < onlineRouteTable[i].mask)
            {
                bestMask = onlineRouteTable[i].mask;
                *macAddr = onlineRouteTable[i].macAddr;
                *device = onlineRouteTable[i].device;
                success = 1;
            }
    pthread_mutex_unlock(&tableMutex);
    if (success)
        return success;

    return 0;
}

int setRoutingTable(uint32_t dest, uint32_t mask,
                    struct MacAddr nextHopMAC, const char *deviceName)
{
    struct Device *device = findDevice(deviceName);
    if (!device)
        return -1;
    if (manRouteTableCnt == 0)
        manRouteTable = (struct RouteEntry *)realloc(manRouteTable, sizeof(struct RouteEntry *) * 16);
    if (manRouteTableCnt >= 16)
    {
        int flag = 0;
        uint32_t num = manRouteTableCnt;
        for (int i = 0; i < 32; i++)
            if (manRouteTableCnt == (1 << i))
                manRouteTable = (struct RouteEntry *)realloc(manRouteTable, 2 * manRouteTableCnt);
    }
    manRouteTable[manRouteTableCnt].device = device;
    manRouteTable[manRouteTableCnt].macAddr = nextHopMAC;
    manRouteTable[manRouteTableCnt].ipAddr = dest;
    manRouteTable[manRouteTableCnt].mask = mask;
    manRouteTable[manRouteTableCnt].dist = 0;
    manRouteTableCnt++;

    return 0;
}

void printRoute()
{
    static double lastPrint = -INFINITY;
    double nowTime = getSecondTime();
    if (nowTime - printInterval < lastPrint)
        return;
    lastPrint = nowTime;

    pthread_mutex_lock(&tableMutex);
    time_t rawtime;
    time(&rawtime);
    printf("Time: %s", asctime(localtime(&rawtime)));

    for (int i = 0; i < onlineRouteTableCnt; i++)
    {
        struct in_addr ipAddr;
        ipAddr.s_addr = onlineRouteTable[i].ipAddr;
        char ipAddressStr[32];
        inet_ntop(AF_INET, &ipAddr, ipAddressStr, sizeof(ipAddressStr));
        printf("IP: %s   dist: %02d  device: %s", ipAddressStr, onlineRouteTable[i].dist, onlineRouteTable[i].device->deviceName);
        printMacAddr(onlineRouteTable[i].macAddr);
        printf("\n");
    }
    printf("\n");
    fflush(stdout);
    pthread_mutex_unlock(&tableMutex);
}

