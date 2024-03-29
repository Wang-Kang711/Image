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


