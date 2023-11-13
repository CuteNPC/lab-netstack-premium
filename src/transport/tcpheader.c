#include "transport/tcpheader.h"
#include <arpa/inet.h>

struct TCPHeader createTCPHeader(Port srcport,
                                 Port desport,
                                 uint32_t seqNum,
                                 uint32_t ackNum,
                                 uint8_t ctrl,
                                 uint16_t window)
{
    struct TCPHeader header;
    header.srcport = srcport;
    header.desport = desport;
    header.seqNum = htonl(seqNum);
    header.ackNum = htonl(ackNum);
    header.ctrl = ctrl;
    header.window = htons(window);
    /*计算checksum*/
    header.hdrLen = (5) << 4;
    header.urgPtr = 0;
    return header;
}
