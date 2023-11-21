#include "transport/tcpheader.h"
#include "network/ipheader.h"
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "utils/callbacklist.h"
#include "pthread.h"
#include "transport/loopthread.h"
#include "transport/mainthread.h"
#include "utils/time.h"
#include "utils/debug.h"
#include <unistd.h>
#include "transport.h"

struct SocketList socketList;
static pthread_mutex_t port_lock;

uint16_t getPort()
{
    static uint16_t _port = 50000;
    uint16_t res;
    pthread_mutex_lock(&port_lock);
    if (_port == (uint16_t)0xffff)
        res = 0;
    else
        res = ++_port;
    pthread_mutex_unlock(&port_lock);
    return res;
}

int sendTCPPacket(struct TCPHeader tcpHeader, struct Socket *socket, const void *data, uint32_t dataLen, int async)
{
    /*外层已经有锁*/
    debugPrintState(tcpHeader.ctrl, 1);
    debugPrint("TCP len = %d", dataLen);
    void *buffer = (void *)malloc(dataLen + 0x100);
    *(struct TCPHeader *)buffer = tcpHeader;
    uint32_t packetLen = sizeof(struct TCPHeader);
    if (data != NULL && dataLen > 0)
    {
        memcpy(buffer + sizeof(struct TCPHeader), data, dataLen);
        packetLen += dataLen;
    }
    sendIPPacket(socket->srcaddr, socket->desaddr, TCP_PROCOTOL, buffer, packetLen, 1); /*tcp的6改成宏定义*/
    return 0;
}

struct Socket *allocSocket()
{
    pthread_mutex_lock(&socketList.mutex);
    struct Socket *newSocket = (struct Socket *)malloc(sizeof(struct Socket));
    memset(newSocket, 0, sizeof(struct Socket));
    pthread_mutex_init(&newSocket->mutex, NULL);
    pthread_mutex_lock(&newSocket->mutex);
    pthread_mutex_init(&newSocket->mutex2, NULL);
    pthread_mutex_lock(&newSocket->mutex2);
    newSocket->fd = socketList.nextFd++;
    newSocket->next = NULL;
    socketList.tail->next = newSocket;
    socketList.tail = socketList.tail->next;
    pthread_mutex_unlock(&socketList.mutex);
    /*newSocket带锁出去*/
    return newSocket;
}

int freeSocket(struct Socket *socket)
{
    pthread_mutex_lock(&socketList.mutex);
    int res = -1;
    for (struct Socket *it = socketList.head;
         it != NULL;
         it = it->next)
    {
        if (it->next == socket)
        {
            it->next = socket->next;
            res = 0;
            // break;
        }
        if (it->next == NULL)
            socketList.tail = it;
    }
    pthread_mutex_unlock(&socketList.mutex);
    if (res)
        return -1;

    pthread_mutex_lock(&socket->mutex);
    if (socket->readBuffer)
        free(socket->readBuffer);
    if (socket->writeBuffer)
        free(socket->writeBuffer);
    pthread_mutex_unlock(&socket->mutex);
    free(socket);

    return 0;
}

struct Socket *findSocket(int fd)
{
    struct Socket *res;
    pthread_mutex_lock(&socketList.mutex);
    // debugPrint("In findSocket");
    for (res = socketList.head->next;
         res != NULL;
         res = res->next)
    {
        pthread_mutex_lock(&res->mutex);
        // debugPrint("try find %p", res);
        if (res->fd == fd)
        {
            pthread_mutex_unlock(&socketList.mutex);
            return res;
        }
        pthread_mutex_unlock(&res->mutex);
        // debugPrint("continue");
    }
    // debugPrint("Out findSocket");
    pthread_mutex_unlock(&socketList.mutex);
    return NULL;
}

int socketLab(int domain, int type, int protocol)
{
    initTransportLayer();
    debugPrint("socketLab(%d, %d, %d)", domain, type, protocol);
    if (!(domain == AF_INET && type == SOCK_STREAM && protocol == 0))
        return -1;
    struct Socket *socket = allocSocket();
    socket->state = ST_CLOSED;
    int res = socket->fd;
    pthread_mutex_unlock(&socket->mutex);
    return res;
}

int bindLab(int socketfd, const struct sockaddr *address, socklen_t address_len)
{
    initTransportLayer();
    const struct sockaddr_in *addr = (const struct sockaddr_in *)address;
    debugPrint("bindLab(%u, %u, %u)", socketfd, addr->sin_addr.s_addr, address_len);
    if (!(address_len == sizeof(struct sockaddr_in) && address->sa_family == AF_INET))
        return -1;
    struct Socket *socket = findSocket(socketfd);
    if (socket == NULL)
        return -1;
    socket->srcaddr = addr->sin_addr.s_addr;
    socket->srcport = addr->sin_port;
    pthread_mutex_unlock(&socket->mutex);
    return 0;
}

int listenLab(int socketfd, int backlog)
{
    initTransportLayer();
    debugPrint("listenLab(%u, %u)", socketfd, backlog);
    struct Socket *socket = findSocket(socketfd);
    if (socket == NULL)
        return -1;
    socket->state = ST_LISTEN;
    pthread_mutex_unlock(&socket->mutex);
    debugPrint("EndListen");
    return 0;
}

int acceptLab(int socketfd, struct sockaddr *address,
              socklen_t *address_len)
{
    initTransportLayer();
    struct Socket *socket = findSocket(socketfd);
    if (socket == NULL)
        return -1;
    pthread_mutex_unlock(&socket->mutex);
    do
    {
        pthread_mutex_lock(&socketList.mutex);
        for (struct Socket *connectSocket = socketList.head->next;
             connectSocket != NULL;
             connectSocket = connectSocket->next)
            if (connectSocket->parentListenFd == socketfd &&
                connectSocket->state == ST_ESTABLISTED)
            {
                connectSocket->parentListenFd = 0;
                pthread_mutex_unlock(&socketList.mutex);
                return connectSocket->fd;
            }
        pthread_mutex_unlock(&socketList.mutex);
    } while (1);
}

int connectLab(int socketfd, const struct sockaddr *address,
               socklen_t address_len)
{
    initTransportLayer();
    debugPrint("connectLab(%u)", socketfd);
    const struct sockaddr_in *address_tcp = (const struct sockaddr_in *)address;
    struct Socket *socket = findSocket(socketfd);
    if (address_len != sizeof(struct sockaddr_in))
        return -1;
    if (address_tcp->sin_family != AF_INET)
        return -1;
    if (socket == NULL)
        return -1;
    if (socket->srcport == 0)
        socket->srcport = htons(getPort());
    if (socket->srcaddr == 0)
        socket->srcaddr = getFirstDevice()->ipAddr;
    socket->desport = address_tcp->sin_port;
    socket->desaddr = address_tcp->sin_addr.s_addr;
    socket->seqNum = -1; // rand();
    socket->ackNum = 0;
    struct TCPHeader tcpHeader = createTCPHeader(socket->srcport,
                                                 socket->desport,
                                                 socket->seqNum,
                                                 0,
                                                 CT_SYN, 0xffff);
    sendTCPPacket(tcpHeader, socket, NULL, 0, 0);
    socket->state = ST_SYS_SENT;
    pthread_mutex_unlock(&socket->mutex);
    pthread_mutex_lock(&socket->mutex2);
    return 0;
}

ssize_t readLab(int fd, void *buf, size_t nbyte)
{
    initTransportLayer();
    debugPrint("readLab(%u)", fd);
    struct Socket *socket = findSocket(fd);
    if (socket == NULL)
        return -1;
    pthread_mutex_unlock(&socket->mutex);
    uint16_t recvSpace;
    uint16_t recvByte;
    /*TODO: 管程并发*/
    do
    {
        recvSpace = socket->recvFront - socket->recvBack;
        recvByte = nbyte > recvSpace ? recvSpace : nbyte;
        if (recvByte > 0)
            break;
        if (socket->state == ST_CLOSED)
            return 0;
    } while (1);
    // if(recvByte > MAX_TCP_LEN)
    // recvByte = MAX_TCP_LEN;
    pthread_mutex_lock(&socket->mutex);
    if (socket->recvBack + recvByte <= 0x10000U)
    {
        memcpy(buf, socket->readBuffer + socket->recvBack, recvByte);
    }
    else
    {
        uint16_t len1 = 0x10000U - socket->recvBack;
        memcpy(buf, socket->readBuffer + socket->recvBack, len1);
        uint16_t len2 = recvByte - len1;
        memcpy(buf + len1, socket->readBuffer, len2);
    }
    socket->recvBack += recvByte;
    pthread_mutex_unlock(&socket->mutex);
    return recvByte;
}

ssize_t writeLab(int fd, const void *buf, size_t nbyte)
{
    initTransportLayer();
    debugPrint("writeLab(%u)", fd);
    struct Socket *socket = findSocket(fd);
    if (socket == NULL)
        return -1;
    if ((socket->state != ST_ESTABLISTED) ||
        (socket->state & CL_CALL) ||
        (socket->clstate & CL_FIN_RECV))
    {
        pthread_mutex_unlock(&socket->mutex);
        return -1;
    }
    uint16_t sendSpace = (socket->ackBack - 1) - (socket->sendFront);
    uint16_t sendByte = nbyte > sendSpace ? sendSpace : nbyte;
    pthread_mutex_unlock(&socket->mutex);
    do
    {
        sendSpace = (socket->ackBack - 1) - (socket->sendFront);
        sendByte = nbyte > sendSpace ? sendSpace : nbyte;
        if (sendByte > 0)
            break;
        if (socket->state == ST_CLOSED)
            return 0;
    } while (1);
    pthread_mutex_lock(&socket->mutex);
    if (socket->sendFront + sendByte <= 0x10000U)
    {
        memcpy(socket->writeBuffer + socket->sendFront, buf, sendByte);
    }
    else
    {
        uint16_t len1 = 0x10000U - socket->sendFront;
        memcpy(socket->writeBuffer + socket->sendFront, buf, len1);
        uint16_t len2 = sendByte - len1;
        memcpy(socket->writeBuffer, buf + len1, len2);
    }
    socket->sendFront += sendByte;
    pthread_mutex_unlock(&socket->mutex);
    return sendByte;
}

int closeLab(int fd)
{
    initTransportLayer();
    debugPrint("closeLab(%u)", fd);
    struct Socket *socket = findSocket(fd);
    if (socket == NULL)
        return -1;
    // if (socket->state != ST_ESTABLISTED)
    //     return -1;
    socket->clstate |= CL_CALL;
    /*等待完全关闭*/
    pthread_mutex_unlock(&socket->mutex);
    while (1) /*改成条件变量*/
    {
        if (socket->clstate == CL_ALL)
            break;
        if (socket->state == ST_CLOSED)
            break;
    }
    sleep(1);
    freeSocket(socket);
    return 0;
}

int getaddrinfoLab(const char *node, const char *service,
                   const struct addrinfo *hints,
                   struct addrinfo **res)
{
    initTransportLayer();
    return 0;
}

void initSocketList()
{
    socketList.head = (struct Socket *)malloc(sizeof(struct Socket));
    socketList.tail = socketList.head;
    socketList.head->next = NULL;
    socketList.nextFd = NETSTACK_FD_START;
    pthread_mutex_init(&socketList.mutex, NULL);
    pthread_mutex_init(&port_lock, NULL);
    return;
}