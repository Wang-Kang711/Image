#ifndef SHARE_H
#define SHARE_H

#include <stdint.h>

#define MAX_FD_LEN 16
#define MAX_MAC_RECORDS 64

typedef struct _SCB {
    unsigned char mac;
    
    int cntARP;
    int cntTCP;
    int cntUDP;
    int cntICMP;
}SCB;

typedef struct _CONTEXT
{
    uint32_t config;
    SCB scb[MAX_MAC_RECORDS];
    int fdList[MAX_FD_LEN];
    int fdIdx;
} CONTEXT;

extern CONTEXT gContext;

#endif