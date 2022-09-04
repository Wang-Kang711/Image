#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include "list1.h"
#include "util.h"
#include "eth.h"
#include "arp.h"
#include "share.h"

#define TRUE 1
#define BUFF_MAX 1518
#define ARP_TYPE 0x0806
#define IP_TYPE 0x0800

extern CONTEXT gContext;

getMax(int *arr, int count)
{
    int max = 0;
    int idx = 0;

    for (idx = 0; idx < count; idx++)
    {
        max = max > arr[idx] ? max : arr[idx];
    }

    return max;
}

static int handleARP(char *buff, int recvdSize) {
    if (recvdSize >= ETH_HEAD_OFFSET + ARP_SIZE) {
        insert(gContext.scb, )
    } else {
        printf("ARP size is not enough!\n");
    }
}

static int handleIP(char *buff, int recvdSize) {
    
}

static int parseMsg(char *buff, int recvdSize) {
    ETH *eth = (ETH *)buff;
    if (eth->eth_type == ARP_TYPE) {
        handleARP(buff, recvdSize);
    } else if (eth->eth_type == IP_TYPE) {

    }
}

int captureSock(int *fdList, int count)
{
    fd_set fdRead = {};
    int readyCnt = 0;
    int idx = 0;
    int maxFd = 0;
    ssize_t recvdSize = 0;
    char buff[BUFF_MAX] = {};

    if (!fdList || count <= 0)
    {
        printf("no fd to capture!\n");
        return -1;
    }

    maxFd = getMax(fdList, count) + 1;

    while (TRUE)
    {
        FD_ZERO(&fdRead);

        for (idx = 0; idx < count; idx++)
        {
            FD_SET(fdList[idx], &fdRead);
        }

        readyCnt = select(maxFd, &fdRead, NULL, NULL, NULL);
        /* 遍历fdList, 对已接受到报文的socket进行处理 */
        for (idx = 0; idx < count && readyCnt > 0; idx++)
        {
            if (FD_ISSET(fdList[idx], &fdRead))
            {
                readyCnt--;
                if ((recvdSize = recv(fdList[idx], buff, BUFF_MAX, 0)) == -1)
                {
                    perror("error recv");
                    closeFdList(fdList, count);
                    exit(-1);
                }

                parseMsg(buff, recvdSize);
            }
        }
    }
}