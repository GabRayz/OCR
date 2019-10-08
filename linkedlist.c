#include <stdlib.h>
#include "separation.h"
#include "linkedlist.h"

LinkedList *list_init()
{
    LinkedList *list = malloc(sizeof(LinkedList));
    list->start = NULL;
    list->end = NULL;

    return list;
}

void list_free(LinkedList *list)
{
    free(list);
}

void list_insert(LinkedList *list, Node *node)
{
    if (list->start == NULL)
    {
        list->start = node;
        list->end = node;
    }
    list->end->next = node;
    list->end = node;
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
    if (!l1->start)
        return l1;
    Node *n = l1->start;
    while (n != NULL)
    {
        n = n->next;
    }
    n->next = l2->start;
    return l1;
}
