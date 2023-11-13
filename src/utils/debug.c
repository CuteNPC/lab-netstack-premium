#include <stdio.h>

#include "utils/debug.h"

int debugPrintState(int num,int send)
{
#ifdef DEBUG
    printf("[DEBUG]: ");
    if(send)
        printf("Send ");
    else
        printf("Recv ");
    printf("Ctrl: ");
    if (num & 32)
        printf("CT_URG ");
    if (num & 16)
        printf("CT_ACK ");
    if (num & 8)
        printf("CT_PSH ");
    if (num & 4)
        printf("CT_RST ");
    if (num & 2)
        printf("CT_SYN ");
    if (num & 1)
        printf("CT_FIN ");
    printf("\n");
    return 1;
#endif
    return 0;
}