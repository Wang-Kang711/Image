#ifndef ARP_H
#define ARP_H

#include <stdint.h>

#define ARP_SIZE 28

typedef struct _ARP
{
    uint16_t htype;
    uint16_t ptype;
    unsigned char hlen;
    unsigned char plen;
    uint16_t oper;
    /* addresses */
    unsigned char sender_ha[6];
    unsigned char sender_pa[4];
    unsigned char target_ha[6];
    unsigned char target_pa[4];
} __attribute__((packed)) ARP;

#endif