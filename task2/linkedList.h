#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct node node_t;
struct node {
    int value;
    node_t *next;
};

typedef struct list {
    node_t *root;
} list_t;

list_t* createList();
void prependNode(list_t* list, int value);
void freeList(list_t* list);


#endif
