#ifndef _NETWORK_ROUTE_H_
#define _NETWORK_ROUTE_H_

#include "link/ethheader.h"
#include "network/ipheader.h"
#include "link/device.h"


struct RouteEntry
{
    IPAddr ipAddr;
    IPAddr mask;
    struct Device *device;
    uint32_t dist;
    struct MacAddr macAddr;
};


int initRouteTable();

int handleRouteTable(const void *buf, int len, struct IpHeader ipHeader, struct Device *device);
int broadcastRouteTable();
int updateRouteTable();
int queryRouteTable(IPAddr ip, struct MacAddr *macAddr, struct Device **device);


int setRoutingTable(uint32_t dest, uint32_t mask,
                    struct MacAddr nextHopMAC, const char *device);
void printRoute();
#endif