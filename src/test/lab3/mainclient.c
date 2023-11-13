#include "transport/transport.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

void setRtTb()
{
    uint8_t ip[4];
    uint8_t mac[6];
    ip[0] = 10;
    ip[1] = 100;
    ip[2] = 1;
    ip[3] = 2;

    mac[0] = 0x46;
    mac[1] = 0x27;
    mac[2] = 0xc2;
    mac[3] = 0x76;
    mac[4] = 0x27;
    mac[5] = 0x2a;
    setRoutingTable(*(uint32_t *)ip, -1, *(struct MacAddr *)mac, "veth1-2");
}

void fun(int socketfd)
{
    char buf[1024];
    int cnt = 10;
    while (cnt--)
    {
        fgets(buf, 1024, stdin);
        if (strlen(buf) <= 1)
            break;
        int a = writeLab(socketfd, buf, strlen(buf));
        printf("%d\n", a);
    }
    sleep(1);
}

int main()
{
    initTransportLayer();
    sleep(2);
    setRtTb();

    int sockfd = socketLab(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        exit(1);
    }

    struct sockaddr_in server_addr, client_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12347);
    inet_pton(AF_INET, "10.100.1.2", &server_addr.sin_addr.s_addr);

    int connect_state = connectLab(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (connect_state == -1)
        exit(1);

    fun(sockfd);

    closeLab(sockfd);
}
