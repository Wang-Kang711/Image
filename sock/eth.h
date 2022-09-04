#ifndef ETH_H
#define ETH_H

#include <stdint.h>

#define ETH_HEAD_OFFSET 14

typedef struct _ETH
{
    unsigned char dest[6];
    unsigned char source[6];
    uint16_t eth_type;
} __attribute__((packed)) ETH;

#endif