/**
 * @file ethheader.h
 * @brief Define the header of an Ethernet frame and some related methods.
 */

#ifndef _LINK_ETHHEADER_H_
#define _LINK_ETHHEADER_H_

#include <sys/types.h>
#include <stdint.h>
#include <netinet/ether.h>

#define ADDR_LEN 6

/**
 * @brief MAC address of a device
 */
struct MacAddr
{
    uint8_t a[ADDR_LEN];
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

/**
 * @brief Print the MAC address
 *
 * @param addr MAC address
 *
 */
void printMacAddr(void *addr);

#endif