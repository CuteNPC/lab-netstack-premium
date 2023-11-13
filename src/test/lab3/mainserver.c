#include "transport/transport.h"
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void setRtTb()
{
    uint8_t ip[4];
    uint8_t mac[6];
    ip[0] = 10;
    ip[1] = 100;
    ip[2] = 1;
    ip[3] = 1;

    mac[0] = 0x4a;
    mac[1] = 0x1f;
    mac[2] = 0xb3;
    mac[3] = 0x02;
    mac[4] = 0x0f;
    mac[5] = 0xec;
    setRoutingTable(*(uint32_t *)ip, -1, *(struct MacAddr *)mac, "veth2-1");
}

void fun(int socketfd)
{
    char buf[1024];
    while (readLab(socketfd, buf, 5))
    {
        printf("\n-------\n");
        printf("%s", buf);
        printf("\n-------\n");
    }
}

int main()
{
    initTransportLayer();
    sleep(2);
    setRtTb();

    int sockfd = socketLab(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        exit(1);

    struct sockaddr_in server_addr, client_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12347);
    inet_pton(AF_INET, "10.100.1.2", &server_addr.sin_addr.s_addr);

    int bind_state = bindLab(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bind_state == -1)
        exit(1);

    int listen_state = listenLab(sockfd, 1024);
    if (listen_state == -1)
        exit(1);

    int new_sockfd;
    socklen_t addr_len = sizeof(client_addr);
    new_sockfd = acceptLab(sockfd, (struct sockaddr *)&client_addr, &addr_len);

    fun(new_sockfd);

    closeLab(new_sockfd);
}

