# 基本功能实现

## list.h

```c
#ifndef _LLIST_H_
#define _LLIST_H_

#define LLIST_FORWARD 1
#define LLIST_BACKWARD 2

//define some function callback
typedef void llist_op(const void *);

typedef int llist_cmp(const void *, const void *);

enum {
    STATE_RUNNING = 1,
    STATE_CANCLED = 2,
    STATE_OVER = 3
};

struct llist_node_st
{
    void *data;
    struct llist_node_st *prve;
    struct llist_node_st *next;
};

typedef struct 
{
    int size;
    struct llist_node_st head;
}LLIST;

LLIST *llist_create(int initsize);

int llist_insert(LLIST *, const void *, int);

void * llist_find(LLIST *, const void *, llist_cmp *);

int llist_fetch(LLIST *, const void *, llist_cmp *, void *);

int llist_delete(LLIST *, const void *, llist_cmp *);

void llist_travel(LLIST *, llist_op *);

void llist_destroy(LLIST *);

#endif
```

## list.c

```c
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "llist.h"

#define NAMESIZE 32

struct score_st
{
    int id;
    char name[NAMESIZE];
    int math;
    int chinese;
};


static void print_s(const void *record)
{
    const struct score_st *r = record;
    printf("%d %s %d %d\n", r->id, r->name, r->math, r->chinese);
}

static int id_cmp(const void *key, const void * record)
{
    const int *k = key;
    const struct score_st *r = record;

    return (*k - r->id);
}

static int name_cmp(const void *key, const void * record)
{
    const char *k = key;
    const struct score_st *r = record;

    return strcmp(k, r->name);
}

int main()
{
    LLIST *handler;
    struct score_st tmp;

    // 创建头节点
    handler = llist_create(sizeof(tmp));
    if(handler == NULL)
        exit(1);
    int ret;

    for (int i = 0; i< 7; i++)
    {
        tmp.id = i;
        snprintf(tmp.name, NAMESIZE, "std_%d",i);
        tmp.math = rand()%100;
        tmp.chinese = rand()%100;
        ret = llist_insert(handler, &tmp, LLIST_BACKWARD);
        if(ret)
            exit(1);
    }


    llist_travel(handler, print_s);


    int id = 3;
    printf("\nid = %d\n", id);
    // struct score_st * data;
    // data = llist_find(handler, &id, id_cmp);
    // if(data == NULL)
    //     printf("can not find data.\n");
    // else
    //     print_s(data);

    // llist_delete(handler, &id, id_cmp);

    char * n = "std_5";
    llist_delete(handler, n, name_cmp);


    // struct score_st a;
    // llist_fetch(handler, &id, id_cmp, &a);
    // print_s(&a);

    printf("\n");

    llist_travel(handler, print_s);


    llist_destroy(handler);


    exit(0);
}
```

## main.c

```c
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "llist.h"

#define NAMESIZE 32

struct score_st
{
    int id;
    char name[NAMESIZE];
    int math;
    int chinese;
};


static void print_s(const void *record)
{
    const struct score_st *r = record;
    printf("%d %s %d %d\n", r->id, r->name, r->math, r->chinese);
}

static int id_cmp(const void *key, const void * record)
{
    const int *k = key;
    const struct score_st *r = record;

    return (*k - r->id);
}

static int name_cmp(const void *key, const void * record)
{
    const char *k = key;
    const struct score_st *r = record;

    return strcmp(k, r->name);
}

int main()
{
    LLIST *handler;
    struct score_st tmp;

    // 创建头节点
    handler = llist_create(sizeof(tmp));
    if(handler == NULL)
        exit(1);
    int ret;

    for (int i = 0; i< 7; i++)
    {
        tmp.id = i;
        snprintf(tmp.name, NAMESIZE, "std_%d",i);
        tmp.math = rand()%100;
        tmp.chinese = rand()%100;
        ret = llist_insert(handler, &tmp, LLIST_BACKWARD);
        if(ret)
            exit(1);
    }


    llist_travel(handler, print_s);


    int id = 3;
    printf("\nid = %d\n", id);
    // struct score_st * data;
    // data = llist_find(handler, &id, id_cmp);
    // if(data == NULL)
    //     printf("can not find data.\n");
    // else
    //     print_s(data);

    // llist_delete(handler, &id, id_cmp);

    char * n = "std_5";
    llist_delete(handler, n, name_cmp);


    // struct score_st a;
    // llist_fetch(handler, &id, id_cmp, &a);
    // print_s(&a);

    printf("\n");

    llist_travel(handler, print_s);


    llist_destroy(handler);


    exit(0);
}
```

---

# 柔性数组形式 +  封装

## list.h

```c
#ifndef _LLIST_H_
#define _LLIST_H_


#define LLIST_FORWARD 1
#define LLIST_BACKWARD 2

//define some callback function 
typedef void llist_op(const void *);

typedef int llist_cmp(const void *, const void *);

struct llist_node_st
{
    struct llist_node_st *prve;
    struct llist_node_st *next;
    // char类型占位符，用于记录data开始的地址，用于实现变长结构体 -- 即柔性数组
    // 柔性数组的好处！ 只用分配一次空间，若采用指针则需要动态分配两次空间，即node一次data一次。
    // 若第二次分配失败则还需将第一步内存空间释放后再次申请
    char data[1];
};

typedef struct llist_head
{
    int size;
    struct llist_node_st head;
    //使用函数指针来实现方法的封装,实现类的方法
    int (*insert)(struct llist_head *, const void *, int);
    void * (*find)(struct llist_head *, const void *, llist_cmp *);
    int (*delete)(struct llist_head *, const void *, llist_cmp *);
    int (*fetch)(struct llist_head *, const void *, llist_cmp *, void *);
    void (*travel)(struct llist_head *, llist_op *);

}LLIST;

LLIST *llist_create(int initsize);
void llist_destroy(LLIST *);

#endif
```

## list.c

```c
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "llist.h"

int llist_insert(LLIST *, const void *, int);
int llist_delete(LLIST *, const void *, llist_cmp *);
void * llist_find(LLIST *, const void *, llist_cmp *);
int llist_fetch(LLIST *, const void *, llist_cmp *, void *);
void llist_travel(LLIST *, llist_op *);

LLIST *llist_create(int initsize)
{
    LLIST *new;

    new = malloc(sizeof(*new));
    if(new == NULL)
        return NULL;

    new->size = initsize;
    new->head.prve = &new->head;
    new->head.next = &new->head;

    new->insert = llist_insert;
    new->delete = llist_delete;
    new->fetch = llist_fetch;
    new->find = llist_find;
    new->travel = llist_travel;

    return new;
}

int llist_insert(LLIST *ptr, const void *data, int mode)
{
    struct llist_node_st *newnode;

    // 使用柔性数组分配空间，即pre + next + data的空间, 长度为0的空间仅仅是用于指向data的指针。
    newnode = malloc(sizeof(*newnode) + ptr->size);
    if(newnode == NULL)
        return -1;

    memcpy(newnode->data, data, ptr->size);

    if(mode == LLIST_FORWARD)
    {   
        newnode->prve = &ptr->head;
        newnode->next = ptr->head.next;
    }
    else if(mode == LLIST_BACKWARD)
    {
        newnode->prve = ptr->head.prve;
        newnode->next = &ptr->head;
    }
    else
    {
        return -3;
    }

    newnode->prve->next = newnode;
    newnode->next->prve = newnode;
    return 0;
}

static struct llist_node_st * find_(LLIST *ptr, const void *key, llist_cmp * cmp)
{
    struct llist_node_st *cur;

    for(cur = ptr->head.next; cur != &ptr->head; cur = cur->next)
    {
        if(cmp(key, cur->data) == 0)
            break;
    }

    return cur;
}

void * llist_find(LLIST *ptr, const void *key, llist_cmp * cmp)
{
    struct llist_node_st *node;
    node = find_(ptr, key, cmp);
    if(node == &ptr->head)
        return NULL;
    return node->data;
}


int llist_fetch(LLIST *ptr, const void *key, llist_cmp * cmp, void *data)
{
    struct llist_node_st *node;
    node = find_(ptr, key, cmp);
    if(node == &ptr->head)
        return -1;

    node->prve->next = node->next;
    node->next->prve = node->prve;

    if(data != NULL)
        memcpy(data, node->data, ptr->size);
    free(node);
    return 0;

}

int llist_delete(LLIST *ptr, const void *key, llist_cmp * cmp)
{
    struct llist_node_st *node;
    node = find_(ptr, key, cmp);
    if(node == &ptr->head)
        return -1;

    node->prve->next = node->next;
    node->next->prve = node->prve;
    free(node);
    return 0;

}

void llist_travel(LLIST *ptr, llist_op * op)
{
    struct llist_node_st *cur;

    for(cur = ptr->head.next; cur != &ptr->head; cur = cur->next)
    {
        op(cur->data);
    }

}

void llist_destroy(LLIST *ptr)
{

    struct llist_node_st *cur, *next;

    for(cur = ptr->head.next; cur != &ptr->head; cur= next)
    {
        next = cur->next;
        free(cur);
    }
    free(ptr);
}
```

 ---

# h文件中避免暴露数据结构（正确封装）

## list.h

```c
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
```

## list.c

```c
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "llist.h"

// 对 struct 类型的定义进行封装
// 声明在h文件，定义在c文件 
// 实现类成员封装
struct llist_node_st
{
    struct llist_node_st *prve;
    struct llist_node_st *next;
    //char类型占位符，用于实现变长结构体
    char data[1];
};


struct llist_head_st
{
    int size;
    struct llist_node_st head;
};



LLIST *llist_create(int initsize)
{
    struct llist_head_st *new;

    new = malloc(sizeof(*new));
    if(new == NULL)
        return NULL;

    new->size = initsize;
    new->head.prve = &new->head;
    new->head.next = &new->head;

    return new;
}

int llist_insert(LLIST *p, const void *data, int mode)
{
    struct llist_node_st *newnode;
    // 由于LLIST类型 在.h 文件中声明未void，因此需要转为实际类型后操作。
    struct llist_head_st *ptr = p;

    newnode = malloc(sizeof(*newnode) + ptr->size);
    if(newnode == NULL)
        return -1;

    memcpy(newnode->data, data, ptr->size);

    if(mode == LLIST_FORWARD)
    {   
        newnode->prve = &ptr->head;
        newnode->next = ptr->head.next;
    }
    else if(mode == LLIST_BACKWARD)
    {
        newnode->prve = ptr->head.prve;
        newnode->next = &ptr->head;
    }
    else
    {
        return -3;
    }

    newnode->prve->next = newnode;
    newnode->next->prve = newnode;
    return 0;
}

static struct llist_node_st * find_(struct llist_head_st *ptr, const void *key, llist_cmp * cmp)
{
    struct llist_node_st *cur;

    for(cur = ptr->head.next; cur != &ptr->head; cur = cur->next)
    {
        if(cmp(key, cur->data) == 0)
            break;
    }

    return cur;
}

void * llist_find(LLIST *p, const void *key, llist_cmp * cmp)
{
    struct llist_node_st *node;
    struct llist_head_st *ptr = p;
    node = find_(ptr, key, cmp);
    if(node == &ptr->head)
        return NULL;
    return node->data;
}


int llist_fetch(LLIST *p, const void *key, llist_cmp * cmp, void *data)
{
    struct llist_node_st *node;
    struct llist_head_st *ptr = p;
    node = find_(ptr, key, cmp);
    if(node == &ptr->head)
        return -1;

    node->prve->next = node->next;
    node->next->prve = node->prve;

    if(data != NULL)
        memcpy(data, node->data, ptr->size);
    free(node);
    return 0;

}

int llist_delete(LLIST *p, const void *key, llist_cmp * cmp)
{
    struct llist_node_st *node;
    struct llist_head_st *ptr = p;
    node = find_(ptr, key, cmp);
    if(node == &ptr->head)
        return -1;

    node->prve->next = node->next;
    node->next->prve = node->prve;
    free(node);
    return 0;

}

void llist_travel(LLIST *p, llist_op * op)
{
    struct llist_node_st *cur;
    struct llist_head_st *ptr = p;

    for(cur = ptr->head.next; cur != &ptr->head; cur = cur->next)
    {
        op(cur->data);
    }

}

void llist_destroy(LLIST *p)
{

    struct llist_node_st *cur, *next;
    struct llist_head_st *ptr = p;

    for(cur = ptr->head.next; cur != &ptr->head; cur= next)
    {
        next = cur->next;
        free(cur);
    }
    free(ptr);
}
```

# 内核实现

## list.h

```c
#ifndef _LINUX_LIST_H_
#define _LINUX_LIST_H_

struct list_head
{
    struct list_head *prev;
    struct list_head *next;
};


#define LIST_HEAD_INIT(name) {&(name), &(name)}

#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)

#define __list_for_each(pos, head) \
    for(pos = (head)->next; pos != (head); pos = pos->next)

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member)({ \
        const typeof(( (type *)0)->member ) *__mptr =(ptr);  \
    (type *)((char *)__mptr - offsetof(type, member) ); })

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

static inline void __list_add(struct list_head *new, 
                            struct list_head *prev,struct list_head *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
    __list_add(new, head, head->next);
}

#endif
```

## main.c

```c
#include<stdio.h>
#include<stdlib.h>

#include"list.h"

#define NAMESIZE 32

struct score_st
{
    int id;
    char name[NAMESIZE];
    struct list_head node;
    int math;
    int chinese;
};

static void print_s(struct score_st *d)
{
    printf("%d %s %d %d\n",d->id, d->name, d->math, d->chinese);
}

int main()
{
    struct score_st *datap;
    struct list_head *cur;

    LIST_HEAD(head);

    for(int i=0;i<7;i++)
    {
        datap = malloc(sizeof(*datap));
        if(datap == NULL)
            exit(1);

        datap->id = i;
        snprintf(datap->name,NAMESIZE,"std_%d", i);
        datap->math = rand() % 100;
        datap->chinese = rand() % 100;

        list_add(&datap->node, &head);
    }

    __list_for_each(cur, &head)
    {
        datap = list_entry(cur, struct score_st, node);    
        print_s(datap);
    }

    exit(0);

}
```
