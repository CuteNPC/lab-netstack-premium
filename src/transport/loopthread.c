#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include "network/ipheader.h"
#include "link/ethheader.h"
#include "transport/tcpheader.h"
#include "network/ippacket.h"
#include "transport/mainthread.h"
#include "transport/loopthread.h"
#include <string.h>
#include <assert.h>
#include "utils/callbacklist.h"
#include "utils/debug.h"
#include "utils/time.h"

TCPPacketReceiveCallback TCPHandleFunList[TCPState_SIZE];

struct CallbackList transportCallbackList;

int TCPHandleClosed(struct Socket *socket,
                    const void *packet,
                    const void *data,
                    int len,
                    struct TCPHeader tcpHeader,
                    struct IpHeader ipHeader)
{
    struct Socket tmpSocket;
    tmpSocket.srcport = tcpHeader.desport;
    tmpSocket.srcaddr = ipHeader.dst;
    tmpSocket.desport = tcpHeader.srcport;
    tmpSocket.desaddr = ipHeader.src;
    return 0;
    struct TCPHeader tcpHeader2 = createTCPHeader(tmpSocket.srcport,
                                                 tmpSocket.desport,
                                                 socket->seqNum,
                                                 socket->ackNum,
                                                 CT_RST,
                                                 0);
    debugPrint2("%d,%d", tmpSocket.ecvBack, tmpSocket.recvFront);
    sendTCPPacket(tcpHeader2, &tmpSocket, NULL, 0, 0);
}

int TCPHandleListen(struct Socket *socket,
                    const void *packet,
                    const void *data,
                    int len,
                    struct TCPHeader tcpHeader,
                    struct IpHeader ipHeader)
{
    if (tcpHeader.ctrl & CT_SYN)
    {
        struct Socket *connectSocket = allocSocket();

        connectSocket->desport = tcpHeader.srcport;
        connectSocket->desaddr = ipHeader.src;
        connectSocket->srcport = socket->srcport;
        connectSocket->srcaddr = socket->srcaddr;
        if (socket->srcaddr == 0)
            connectSocket->srcaddr = ipHeader.dst;
        connectSocket->seqNum = -1; // rand();
        connectSocket->ackNum = ntohl(tcpHeader.seqNum) + 1;
        connectSocket->clstate = 0;

        struct TCPHeader tcpHeader = createTCPHeader(connectSocket->srcport,
                                                     connectSocket->desport,
                                                     connectSocket->seqNum,
                                                     connectSocket->ackNum,
                                                     CT_SYN | CT_ACK, 0xffff);
        sendTCPPacket(tcpHeader, connectSocket, NULL, 0, 0);
        // sendTCPPacket(tcpHeader, connectSocket, NULL, 0, 0);
        // sendTCPPacket(tcpHeader, connectSocket, NULL, 0, 0);
        // sendTCPPacket(tcpHeader, connectSocket, NULL, 0, 0);
        // sendTCPPacket(tcpHeader, connectSocket, NULL, 0, 0);
        connectSocket->state = ST_SYS_RECV;
        connectSocket->parentListenFd = socket->fd;
        pthread_mutex_unlock(&connectSocket->mutex);
    }
    /*收到其他的，不理*/

    return 0;
}

int TCPHandleSyssent(struct Socket *socket,
                     const void *packet,
                     const void *data,
                     int len,
                     struct TCPHeader tcpHeader,
                     struct IpHeader ipHeader)
{
    if ((tcpHeader.ctrl & CT_SYN) && (tcpHeader.ctrl & CT_ACK))
    {
        socket->buf_size = TCP_BUFFER_SIZE;
        socket->writeBuffer = malloc(socket->buf_size);
        socket->readBuffer = malloc(socket->buf_size);
        socket->ackNum = ntohl(tcpHeader.seqNum) + 1;
        socket->seqNum += 1;

        socket->sendFront = 0;
        socket->sendBack = 0;
        socket->ackFront = -1;
        socket->ackBack = 0;

        socket->recvFront = 0;
        socket->recvBack = 0;
        debugPrint2("TEST 88");
        struct TCPHeader tcpHeader = createTCPHeader(socket->srcport,
                                                     socket->desport,
                                                     0,
                                                     socket->ackNum,
                                                     CT_ACK, 0xffff);
        sendTCPPacket(tcpHeader, socket, NULL, 0, 0);
        socket->isConnection = 1;
        socket->state = ST_ESTABLISTED;
        pthread_mutex_unlock(&socket->mutex2);
    }
    return 0;
}

int TCPHandleSysrecv(struct Socket *socket,
                     const void *packet,
                     const void *data,
                     int len,
                     struct TCPHeader tcpHeader,
                     struct IpHeader ipHeader)
{
    if (tcpHeader.ctrl & CT_ACK)
    {
        socket->buf_size = TCP_BUFFER_SIZE;
        socket->readBuffer = malloc(socket->buf_size);
        socket->writeBuffer = malloc(socket->buf_size);
        socket->seqNum += 1;

        socket->sendFront = 0;
        socket->sendBack = 0;
        socket->ackFront = -1;
        socket->ackBack = 0;

        socket->recvBack = 0;
        socket->recvFront = 0;
        socket->state = ST_ESTABLISTED;
        socket->isConnection = 1;
    }

    return 0;
}

int TCPHandleEstablished(struct Socket *socket,
                         const void *packet,
                         const void *data,
                         int len,
                         struct TCPHeader tcpHeader,
                         struct IpHeader ipHeader)
{
    tcpHeader.seqNum = htonl(tcpHeader.seqNum);
    tcpHeader.ackNum = htonl(tcpHeader.ackNum);
    tcpHeader.window = htons(tcpHeader.window);

    if (tcpHeader.ctrl & CT_ACK)
    {
        uint16_t ack16 = tcpHeader.ackNum;
        if (ack16 == socket->sendBack) // && ack16 != socket->ackBack)
        {
            uint16_t ackDiff = tcpHeader.ackNum - socket->seqNum;
            socket->ackBack += ackDiff;
            assert(socket->ackBack == (uint16_t)tcpHeader.ackNum);
            socket->ackFront = socket->ackBack + tcpHeader.window;
            socket->seqNum = tcpHeader.ackNum;
            debugPrint2("tcpHeader.window %u", tcpHeader.window);
            debugPrint2("TCPHandleEstablished %u,%u,%u,%u", socket->ackBack, socket->sendBack, socket->sendFront, socket->ackFront);
        }
        if (socket->clstate & CL_FIN_SENT)
            socket->clstate |= CL_ACK_RECV;
        if ((socket->clstate & CL_CLOSE) == CL_CLOSE)
        {
            socket->isConnection = 0;
            socket->state = ST_CLOSED;
        }
    }
    if (tcpHeader.ctrl & CT_FIN)
    {
        /*此时禁止我方调用write函数*/
        socket->clstate |= CL_FIN_RECV;
        debugPrint2("TEST 167");
        struct TCPHeader tcpHeader = createTCPHeader(socket->srcport,
                                                     socket->desport,
                                                     socket->seqNum,
                                                     socket->ackNum + 1,
                                                     CT_ACK,
                                                     socket->recvBack - 1 - socket->recvFront);
        socket->clstate |= CL_ACK_SENT;
        sendTCPPacket(tcpHeader, socket, NULL, 0, 0);
    }
    if (len > 0 && tcpHeader.seqNum == socket->ackNum)
    {
        /*审查应该接受的字节数目*/
        uint16_t receivedLen = len;
        uint16_t maxAvaiable = socket->recvBack - 1 - socket->recvFront;
        uint16_t validLen = maxAvaiable < receivedLen ? maxAvaiable : receivedLen;
        assert(validLen == receivedLen);
        if (socket->recvFront + validLen <= 0x10000U)
        {
            memcpy(socket->readBuffer + socket->recvFront, data, validLen);
        }
        else
        {
            uint16_t len1 = 0x10000U - socket->recvFront;
            memcpy(socket->readBuffer + socket->recvFront, data, len1);
            uint16_t len2 = validLen - len1;
            memcpy(socket->readBuffer, data + len1, len2);
        }
        socket->ackNum = tcpHeader.seqNum + validLen;
        socket->recvFront += validLen;
        debugPrint2("TEST 199");
        struct TCPHeader tcpHeader = createTCPHeader(socket->srcport,
                                                     socket->desport,
                                                     socket->seqNum,
                                                     socket->ackNum,
                                                     CT_ACK,
                                                     socket->recvBack - 1 - socket->recvFront);
        debugPrint2("%d,%d", socket->recvBack, socket->recvFront);
        sendTCPPacket(tcpHeader, socket, NULL, 0, 0);
    }
    else if(tcpHeader.ctrl == 0 && tcpHeader.seqNum == socket->ackNum)
    {
        debugPrint2("TEST 211");
        struct TCPHeader tcpHeader = createTCPHeader(socket->srcport,
                                                     socket->desport,
                                                     socket->seqNum,
                                                     socket->ackNum,
                                                     CT_ACK,
                                                     socket->recvBack - 1 - socket->recvFront);
        sendTCPPacket(tcpHeader, socket, NULL, 0, 0);
    }
}

int initTCPHandleFunList()
{
    TCPHandleFunList[ST_CLOSED] = TCPHandleClosed;

    TCPHandleFunList[ST_LISTEN] = TCPHandleListen;
    TCPHandleFunList[ST_SYS_SENT] = TCPHandleSyssent;
    TCPHandleFunList[ST_SYS_RECV] = TCPHandleSysrecv;

    TCPHandleFunList[ST_ESTABLISTED] = TCPHandleEstablished;

    TCPHandleFunList[ST_CLOSE_WAIT] = NULL;
    TCPHandleFunList[ST_LAST_ACK] = NULL;

    TCPHandleFunList[ST_FIN_WAIT1] = NULL;
    TCPHandleFunList[ST_FIN_WAIT2] = NULL;
    TCPHandleFunList[ST_CLOSING] = NULL;
    TCPHandleFunList[ST_TIME_WAIT] = NULL;

    return 0;
}

struct Socket *matchSocket(struct TCPHeader tcpHeader, struct IpHeader ipHeader)
{
    pthread_mutex_lock(&socketList.mutex);
    for (struct Socket *it = socketList.head->next;
         it != NULL;
         it = it->next)
    {
        pthread_mutex_lock(&it->mutex);
        if (tcpHeader.desport == it->srcport &&
            ipHeader.dst == it->srcaddr &&
            tcpHeader.srcport == it->desport &&
            ipHeader.src == it->desaddr)
        {
            pthread_mutex_unlock(&socketList.mutex);
            return it;
        }
        pthread_mutex_unlock(&it->mutex);
    }
    for (struct Socket *it = socketList.head->next;
         it != NULL;
         it = it->next)
    {
        pthread_mutex_lock(&it->mutex);
        if (tcpHeader.desport == it->srcport &&
            // ipHeader.dst == it->srcaddr &&
            it->state == ST_LISTEN)
        {
            pthread_mutex_unlock(&socketList.mutex);
            return it;
        }
        pthread_mutex_unlock(&it->mutex);
    }
    debugPrint("matchSocket:Not found");
    pthread_mutex_unlock(&socketList.mutex);
    return NULL;
}

int handleTCPMain(struct Socket *match, const void *packet, const void *data, int len, struct TCPHeader tcpHeader, struct IpHeader ipHeader)
{
    debugPrint("handleTCPMain");
    match = matchSocket(tcpHeader, ipHeader);
    if (match == NULL)
    {
        return -1;
    }
    debugPrint("(%u, %u, %u)", match->state, tcpHeader.seqNum, tcpHeader.ackNum);
    debugPrintState(tcpHeader.ctrl, 0);
    TCPHandleFunList[match->state](match, packet, data, len, tcpHeader, ipHeader);
    pthread_mutex_unlock(&match->mutex);
}

void asyncSendTCPPacket()
{
    static int cnt = 0;
    pthread_mutex_lock(&socketList.mutex);
    // debugPrint("asyncSendTCPPacket");
    for (struct Socket *it = socketList.head->next;
         it != NULL;
         it = it->next)
    {
        pthread_mutex_lock(&it->mutex);
        if (!it->isConnection) /*怎么判断？*/
        {
            pthread_mutex_unlock(&it->mutex);
            continue;
        }
        int send = 0;
        uint16_t start = 0;
        uint16_t end = 0;
        double nowtime = getSecondTime();
        if (it->sendBack != it->ackBack)
        {
            if (it->resendTime < nowtime)
            {
                debugPrint("asyncSendTCPPacket resend");
                // printf("asyncSendTCPPacket resend");
                it->resendTime = nowtime + 0.5;
                send = 1;
                start = it->ackBack;
                end = it->sendBack;
                // printf("RESEND!!!");
                // exit(-1);
            }
        }
        else if (it->ackBack == it->ackFront)
        {
            if (it->resendTime < nowtime)
            {
                debugPrint("Try resend");
                it->resendTime = nowtime + 0.5;
                send = 1;
                start = it->ackBack;
                end = it->sendBack;
            }
        }
        else
        {
            uint16_t len1 = it->sendFront - it->sendBack;
            uint16_t len2 = it->ackFront - it->sendBack;
            uint16_t len = len1 < len2 ? len1 : len2;
            if (len > 0)
            {
                // if (++cnt == 80)
                // {
                // int a = 1;
                // }
                debugPrint("asyncSendTCPPacket send");
                send = 1;
                start = it->sendBack;
                if (len > MAX_TCP_LEN)
                    len = MAX_TCP_LEN;
                it->sendBack += len;
                end = it->sendBack;
                it->resendTime = nowtime + 0.5;
                debugPrint2("asyncSendTCPPacket %u,%u,%u,%u", it->ackBack, it->sendBack, it->sendFront, it->ackFront);
            }
        }
        if (send)
        {
            uint8_t data[0x10000];
            uint16_t dataLen = end - start;
            debugPrint("end = %d, start = %d, dataLen = %d", end, start, dataLen);
            if (end >= start)
            {
                memcpy(data, it->writeBuffer + start, end - start);
            }
            else
            {
                uint16_t len1 = 0x10000 - start;
                memcpy(data, it->writeBuffer + start, len1);
                memcpy(data + len1, it->writeBuffer, end);
            }

            struct TCPHeader tcpHeader = createTCPHeader(it->srcport,
                                                         it->desport,
                                                         it->seqNum,
                                                         it->ackNum,
                                                         0, 0xffff);
            sendTCPPacket(tcpHeader, it, data, dataLen, 0);
        }
        send = 0;
        if (it->ackBack == it->sendBack && it->sendBack == it->sendFront)
        {
            if ((it->clstate & CL_FIN_RECV) && (it->clstate & CL_ACK_SENT) || (it->clstate & CL_CALL))
            {
                if (!(it->clstate & CL_FIN_SENT))
                {
                    debugPrint("asyncSendTCPPacket send fin");
                    send = 1;
                    it->resendTime = nowtime + 5;
                }
                else if ((!(it->clstate & CL_ACK_RECV)) && (it->resendTime < nowtime))
                {
                    debugPrint("asyncSendTCPPacket resend fin");
                    send = 1;
                    it->resendTime = nowtime + 5;
                }
                if (send)
                {
                    struct TCPHeader tcpHeader = createTCPHeader(it->srcport,
                                                                 it->desport,
                                                                 it->seqNum,
                                                                 it->ackNum,
                                                                 CT_FIN, 0xffff);
                    sendTCPPacket(tcpHeader, it, NULL, 0, 0);
                    it->clstate |= CL_FIN_SENT;
                }
            }
        }
        pthread_mutex_unlock(&it->mutex);
    }
    pthread_mutex_unlock(&socketList.mutex);
}

int handleTCPPacket(const void *packet, uint32_t pktlen, struct IpHeader ipHdr, struct Device *device)
{
    if (ipHdr.protocol != TCP_PROCOTOL)
        return -1;
    debugPrint("handleTCPPacket");
    /* Extract the frame header and data separately */
    struct TCPHeader hdr = *(struct TCPHeader *)packet;
    const void *data = packet + sizeof(struct IpHeader);
    uint32_t len = pktlen - sizeof(struct IpHeader);

    // if (!isMyPacket(hdr))
    // deliverIPPacket(packet, pktlen, hdr.dst);

    if (!transportCallbackList.head)
        return -1;

    for (struct CallbackNode *p = transportCallbackList.head->next;
         p != NULL; p = p->next)
        ((TCPPacketReceiveCallback)p->funcPtr)(NULL, packet, data, len, hdr, ipHdr);

    return 0;
}

int setTCPPacketReceiveCallback(TCPPacketReceiveCallback callback)
{
    static int _initialized = 0;
    if (_initialized == 0)
    {
        initCallbackList(&transportCallbackList);
        _initialized = 1;
    }
    insertCallback(&transportCallbackList, (void *)callback);
}
