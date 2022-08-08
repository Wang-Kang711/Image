#ifndef _LLIST_H_
#define _LLIST_H_


#define LLIST_FORWARD 1
#define LLIST_BACKWARD 2

// 封装 节点与头节点的定义。
// 仅暴露用户需要用到的数据类型，同时因为未暴露头节点类型，采用通用void替代。
typedef void LLIST;

//define some callback function 
typedef void llist_op(const void *);

typedef int llist_cmp(const void *, const void *);


int llist_insert(LLIST *, const void *, int);
int llist_delete(LLIST *, const void *, llist_cmp *);
void * llist_find(LLIST *, const void *, llist_cmp *);
int llist_fetch(LLIST *, const void *, llist_cmp *, void *);
void llist_travel(LLIST *, llist_op *);
LLIST * llist_create(int initsize);
void llist_destroy(LLIST *);

#endif
