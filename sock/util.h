#ifndef UTIL_H
#define UTIL_H

#define BIT_CONTAIN(num, bit) ((num) & (1 << bit))
#define BIT_SET(num, bit) ((num) |= (1 << bit))
#define BIT_CLR(num, bit) ((num) &= ~(1 << bit))

void closeFdList(int *fdList, int count);

#endif