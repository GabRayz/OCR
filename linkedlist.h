# ifndef LINKED_LIST_H
# define LINKED_LIST_H

typedef struct _Block
{
    int x;
    int y;
    int width;
    int height;
} Block;

typedef struct _Node
{
    Block *block;
    struct _Node *next;
    struct _Node *previous;
} Node;

typedef struct _LinkedList
{
    Node *start;
    Node *end;
} LinkedList;

LinkedList *list_init();
void list_free(LinkedList *list);
void list_insert(LinkedList *list, Node *node);
int list_length(LinkedList *list);
LinkedList *list_concat(LinkedList *l1, LinkedList *l2);
Node *node_init(Block *block);
void node_free(Node *node);
void list_link_nodes(Node *a, Node *b);
void list_insert_list(Node *source, LinkedList *list);
Block *list_get_index(LinkedList *list, int index);

#endif
