#include <stdlib.h>
#include "linkedlist.h"
#include <assert.h>
#include <stdio.h>

LinkedList *list_init()
{
    LinkedList *list = malloc(sizeof(LinkedList));
    list->start = NULL;
    list->end = NULL;

    return list;
}

void list_free(LinkedList *list)
{
    assert(list != NULL);
    free(list);
}

Node *node_init(void *data)
{
    Node *node = malloc(sizeof(Node));
    node->data = data;
    node->next = NULL;
    node->previous = NULL;

    return node;
}

void node_free(Node *node)
{
    assert(node != NULL);
    free(node);
}

void list_insert(LinkedList *list, Node *node)
{
    if (list->start == NULL)
    {
        list->start = node;
        list->end = node;
    }
    else
    {
        node->previous = list->end;
        list->end->next = node;
        list->end = node;
    }
}

void list_remove(LinkedList *list, Node *node)
{
    assert(list != NULL && node != NULL);

    if (list->start == node)
        list->start = node->next;

    if (list->end == node)
        list->end = node->previous;

    if (node->previous != NULL)
        node->previous->next = node->next;

    if (node->next != NULL)
        node->next->previous = node->previous;

    node_free(node);
}

int list_length(LinkedList *list)
{
    int res = 0;
    Node *node = list->start;
    while (node != NULL)
    {
        node = node->next;
        res++;
    }
    return res;
}

LinkedList *list_concat(LinkedList *l1, LinkedList *l2)
{
    assert(l1 != NULL && l2 != NULL);
    if (l1->start == NULL)
    {
        list_free(l1);
        return l2;
    }

    assert(l1->end != NULL);

    l1->end->next = l2->start;
    if (l2->start)
    {
        l2->start->previous = l1->end;
        l1->end = l2->end;
    }

    list_free(l2);
    return l1;
}

void *list_get_index(LinkedList *list, int index)
{
    Node *node = list->start;
    int i = 0;
    for (i = 0; i < index && node; i++)
    {
        node = node->next;
    }
    if (i == index)
        return node->data;
    else
        return NULL;
}

void **list_to_array(LinkedList *list)
{
    void **res = malloc(sizeof(void *) * (list_length(list) + 1));
    Node *n = list->start;
    int i = 0;
    while (n)
    {
        res[i] = n->data;
        n = n->next;
        i++;
    }
    res[i] = NULL;

    return res;
}
