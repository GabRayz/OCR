#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "separation.h"

typedef struct _Node
{
    Block *block;
    struct _Node *next;
    struct _Node *previous;
} Node;

typedef struct _LinkedList {
    Node *start;
    Node *end;
} LinkedList;

LinkedList *list_init();
void list_free(LinkedList *list);
void list_insert(LinkedList *list, Node *node);
int list_length(LinkedList *list);
LinkedList *list_concat(LinkedList *l1, LinkedList *l2);
Node *node_init(Block *block);
void list_link_nodes(Node *a, Node *b);
void list_insert_list(Node *source, LinkedList *list);

#endif
