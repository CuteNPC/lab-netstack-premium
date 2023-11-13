#ifndef _TRANSPORT_LABSOCKET_H_
#define _TRANSPORT_LABSOCKET_H_

#include "network/ipheader.h"
#include "link/ethheader.h"
#include "transport/tcpheader.h"
#include "network/ippacket.h"

#define NETSTACK_FD_START (123456789)
#define TCP_BUFFER_SIZE (0x10000)

enum TCPState
{
    ST_CLOSED,

    ST_LISTEN,
    ST_SYS_SENT,
    ST_SYS_RECV,

    ST_ESTABLISTED,

    ST_CLOSE_WAIT,
    ST_LAST_ACK,

    ST_FIN_WAIT1,
    ST_FIN_WAIT2,
    ST_CLOSING,
    ST_TIME_WAIT,

    TCPState_SIZE,
};

enum TCPCtrlBit
{
    CT_URG = 32,
    CT_ACK = 16,
    CT_PSH = 8,
    CT_RST = 4,
    CT_SYN = 2,
    CT_FIN = 1,
};

enum CLOSEState
{
    CL_ZREO = 0,
    CL_CALL = 1,
    CL_FIN_SENT = 2,
    CL_ACK_SENT = 4,
    CL_FIN_RECV = 8,
    CL_ACK_RECV = 16,
    CL_CLOSE = 30,
    CL_ALL = 31,
};

struct Socket
{
    int fd;

    IPAddr desaddr;
    Port desport;
    IPAddr srcaddr;
    Port srcport;

    enum TCPState state;
    enum CLOSEState clstate;

    void *readBuffer;
    void *writeBuffer;
    ssize_t buf_size;

    uint16_t sendFront;
    uint16_t sendBack ;
    uint16_t recvFront;
    uint16_t recvBack ;
    uint16_t ackFront ;
    uint16_t ackBack  ;

    // ssize_t sendWindow;
    // ssize_t recvWindow;
    uint32_t seqNum;
    uint32_t ackNum;
    double resendTime;

    struct Socket *next;

    pthread_mutex_t mutex;

    pthread_mutex_t mutex2;

    int isConnection;
    int parentListenFd;
};

struct SocketList
{
    struct Socket *head;
    struct Socket *tail;
    int nextFd;
    pthread_mutex_t mutex;
};

extern struct SocketList socketList;
struct Socket *allocSocket();
int socketLab(int domain, int type, int protocol);
int bindLab(int socketfd, const struct sockaddr *address, socklen_t address_len);
int listenLab(int socketfd, int backlog);
int acceptLab(int socketfd, struct sockaddr *address,
              socklen_t *address_len);
int connectLab(int socket, const struct sockaddr *address,
               socklen_t address_len);
ssize_t readLab(int fildes, void *buf, size_t nbyte);
ssize_t writeLab(int fildes, const void *buf, size_t nbyte);
int closeLab(int fildes);
int getaddrinfoLab(const char *node, const char *service,
                   const struct addrinfo *hints,
                   struct addrinfo **res);

int sendTCPPacket(struct TCPHeader tcpHeader, struct Socket *socket, const void *data, uint32_t dataLen, int async);
void initSocketList();
#endif
