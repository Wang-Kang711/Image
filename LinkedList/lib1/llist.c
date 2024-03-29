#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "llist.h"

/**
 * @brief  创建头节点
 * @note
 * @param  initsize: 储存node所需长度
 * @retval 头节点指针
 */
LLIST *llist_create(int initsize)
{
    // 申请LLIST一块空间，并用new指针指向
    LLIST *new = malloc(sizeof(*new));
    if (!new)
    {
        return NULL;
    }

    new->size = initsize;
    new->head.data = NULL;
    new->head.prve = &new->head;
    new->head.next = &new->head;

    return new;
}

int llist_insert(LLIST *ptr, const void *data, int mode)
{
    struct llist_node_st *newnode;

    if (!ptr) {
        return -2;
    }

    newnode = malloc(sizeof(*newnode));
    if (newnode == NULL)
        // 此处可采用 枚举
        return STATE_OVER;

    newnode->data = malloc(ptr->size);
    if (newnode == NULL)
        return -2;

    memcpy(newnode->data, data, ptr->size);

    if (mode == LLIST_FORWARD)
    {
        newnode->prve = &ptr->head;
        newnode->next = ptr->head.next;
    }
    else if (mode == LLIST_BACKWARD)
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

static struct llist_node_st *find_(LLIST *ptr, const void *key, llist_cmp *cmp)
{
    struct llist_node_st *cur;

    for (cur = ptr->head.next; cur != &ptr->head; cur = cur->next)
    {
        if (cmp(key, cur->data) == 0)
            break;
    }

    return cur;
}

void *llist_find(LLIST *ptr, const void *key, llist_cmp *cmp)
{
    return find_(ptr, key, cmp)->data;
}

int llist_fetch(LLIST *ptr, const void *key, llist_cmp *cmp, void *data)
{
    struct llist_node_st *node;
    node = find_(ptr, key, cmp);
    if (node == &ptr->head)
        return -1;

    node->prve->next = node->next;
    node->next->prve = node->prve;

    if (data != NULL)
        memcpy(data, node->data, ptr->size);
    free(node->data);
    free(node);
    return 0;
}

int llist_delete(LLIST *ptr, const void *key, llist_cmp *cmp)
{
    struct llist_node_st *node;
    node = find_(ptr, key, cmp);
    if (node == &ptr->head)
        return -1;

    node->prve->next = node->next;
    node->next->prve = node->prve;
    free(node->data);
    free(node);
    return 0;
}

void llist_travel(LLIST *ptr, llist_op *op)
{
    struct llist_node_st *cur;

    for (cur = ptr->head.next; cur != &ptr->head; cur = cur->next)
    {
        op(cur->data);
    }
}

void llist_destroy(LLIST *ptr)
{

    struct llist_node_st *cur, *next;

    for (cur = ptr->head.next; cur != &ptr->head; cur = next)
    {
        next = cur->next;
        free(cur->data);
        free(cur);
    }
    free(ptr);
}
