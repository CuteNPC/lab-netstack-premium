/**
 * @file ethheader.h
 * @brief Define the header of an Ethernet frame and some related methods.
 */

#ifndef _LINK_ETHHEADER_H_
#define _LINK_ETHHEADER_H_

#include <sys/types.h>
#include <stdint.h>
#include <netinet/ether.h>

#define MAC_ADDR_LEN 6
#define ETHTYPE_IPv4 0x0800
#define ETHTYPE_IPv6 0x86DD
#define ETHTYPE_ARP 0x0806

/**
 * @brief MAC address of a device
 */
struct MacAddr
{
    uint8_t a[MAC_ADDR_LEN];
} __attribute__((__packed__));

/**
 * @brief Header of the Ethernet II frame.
 */
struct EthHeader
{
    struct MacAddr desAddr;
    struct MacAddr srcAddr;
    uint16_t type;
} __attribute__((__packed__));

/**
 * @brief Create a Ethernet II frame header.
 *
 * @param desAddr Destination address of the Ethernet II frame
 * @param srcAddr Source address of the Ethernet II frame
 * @param type Ethernet II frame type
 * @return Ethernet II frame header.
 *
 */
struct EthHeader createEthHeader(struct MacAddr desAddr,
                                 struct MacAddr srcAddr,
                                 uint16_t type);

int macAddrEqual(struct MacAddr a, struct MacAddr b);

extern struct MacAddr BROAD_MAC;
extern struct MacAddr ZREO_MAC;

/**
 * @brief Print the MAC address
 *
 * @param addr MAC address
 *
 */
void printMacAddr(struct MacAddr desAddr);

#endif