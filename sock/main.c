#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include "util.h"
#include "share.h"

#define ARP_BIT_LOC 3
#define UDP_BIT_LOC 2
#define TCP_BIT_LOC 1
#define ICMP_BIT_LOC 0
#define INTERFACE_ETH0 2
#define SUCCESS 1
#define FAIL -1

typedef struct _CONTEXT
{
    uint32_t config;
    int fdList[MAX_FD_LEN];
    int fdIdx;
} CONTEXT;

CONTEXT gContext = {};

void usage()
{
    printf("usage\n");
}

static void handleParam(int bitIndex)
{
    if (BIT_CONTAIN(gContext.config, bitIndex))
    {
        printf("[FUNCTION:%s, LINE:%d]:重复参数\n", __FUNCTION__, __LINE__);
        usage();
        exit(-1);
    }

    BIT_SET(gContext.config, bitIndex);
}

static void parseConfig(char argc, char **argv)
{
    int opt;

    if (argc == 1)
    {
        gContext.config = 0x0f;
    }

    while ((opt = getopt(argc, argv, "autih")) != -1)
    {
        if (opt == 'a')
        {
            handleParam(ARP_BIT_LOC);
        }
        else if (opt == 'u')
        {
            handleParam(UDP_BIT_LOC);
        }
        else if (opt == 't')
        {
            handleParam(TCP_BIT_LOC);
        }
        else if (opt == 'i')
        {
            handleParam(ICMP_BIT_LOC);
        }
        else if (opt == 'h')
        {
            usage();
        }
        else
        {
            printf("[FUNCTION:%s, LINE:%d]:未知参数\n", __FUNCTION__, __LINE__);
            break;
        }
    }

    printf("[FUNCTION:%s, LINE:%d]:解析入参完成, config:%#x\n", __FUNCTION__, __LINE__, gContext.config);
}

static int createRawSock(int interface, int protocol)
{
    struct sockaddr_ll sock = {};
    int fd = -1;

    /* 创建socket */
    fd = socket(AF_PACKET, SOCK_RAW, htons(protocol));
    if ((fd = socket(AF_PACKET, SOCK_RAW, htons(protocol))) < 0)
    {
        perror("create sock error!");
        return FAIL;
    }

    sock.sll_family = AF_PACKET;
    sock.sll_protocol = htons(protocol);
    sock.sll_ifindex = interface; /* 监控eth0网卡 */

    /* 绑定socket与网卡等 */
    if (bind(fd, (struct sockaddr *)&sock, sizeof(sock)) == -1)
    {
        perror("bind sock error!");
        return FAIL;
    }

    return fd;
}

static inline manageFd(int interface, int protocol)
{
    int fd = -1;
    int idx = 0;

    if ((fd = createRawSock(interface, protocol)) == FAIL)
    {
        /* 若创建socket失败，则释放已创建socket */
        closeFdList(&gContext.fdList, gContext.fdIdx);
    }
    else
    {
        gContext.fdList[gContext.fdIdx] = fd;
        gContext.fdIdx++;
        printf("create socket success!\n");
    }
}

int main(char argc, char **argv)
{
    /* 解析 */
    parseConfig(argc, argv);

    /* 创建信号量 */

    /* 创建socket */
    /* 创建ARP socket_raw */
    if (BIT_CONTAIN(gContext.config, ARP_BIT_LOC))
    {
        manageFd(INTERFACE_ETH0, ETH_P_ARP);
    }
    /* 若需要统计TCP/UDP/ICMP任一报文，则创建IP socket_raw */
    if (BIT_CONTAIN(gContext.config, TCP_BIT_LOC) || BIT_CONTAIN(gContext.config, UDP_BIT_LOC) || BIT_CONTAIN(gContext.config, ICMP_BIT_LOC))
    {
        manageFd(INTERFACE_ETH0, ETH_P_ARP);
    }

    /* 捕获并处理报文 */
    captureSock(&gContext.fdList, gContext.fdIdx);
}