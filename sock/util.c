#include <stdio.h>
#include <linux/limits.h>
#include "util.h"

void closeFdList(int *fdList, int count)
{
    int idx = 0;

    /* 若创建socket失败，则释放已创建socket */
    for (idx = 0; idx < count; idx++)
    {
        close(fdList[idx]);
    }
    exit(-1);
}