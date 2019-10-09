#include <stdlib.h>
#include "separation.h"
#include "linkedlist.h"
#include <assert.h>

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
    node->previous = list->end;
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
    assert(l1 != NULL && l2 != NULL);

    if (!l1->start) {
        list_free(l1);
        return l2;
    }

    assert(l1->end != NULL);

    l1->end->next = l2->start;
    if (l2->start) {
        l2->start->previous = l1->end->next;
        l1->end = l2->end;
    }

    list_free(l2);
    return l1;
}
