#ifndef _TRANSPORT_TCPHEADER_H_
#define _TRANSPORT_TCPHEADER_H_

#include <stdint.h>
#include "network/ipheader.h"

#define TCP_PROCOTOL (6)
#define MAX_TCP_LEN (1024)

typedef uint16_t Port;

struct TCPHeader
{
    Port srcport;
    Port desport;
    uint32_t seqNum;
    uint32_t ackNum;
    uint8_t hdrLen;
    uint8_t ctrl;
    uint16_t window;
    uint16_t checkSum;
    uint16_t urgPtr;
};

struct TCPHeader createTCPHeader(Port srcport,
                                 Port desport,
                                 uint32_t seqNum,
                                 uint32_t ackNum,
                                 uint8_t ctrl,
                                 uint16_t window);

#endif
