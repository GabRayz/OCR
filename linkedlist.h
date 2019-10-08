#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "separation.h"

typedef struct _Node
{
    Block *block;
    struct _Node *next;
} Node;

typedef struct _LinkedList {
    Node *start;
    Node *end;
} LinkedList;

LinkedList *list_init();
void list_free(LinkedList *list);
void list_insert(LinkedList *list, Node *node);
int list_length(LinkedList *list);


#endif
