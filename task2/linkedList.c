#include "linkedList.h"
#include <stdio.h>
#include <stdlib.h>

list_t* createList(){
    list_t* newList = (list_t *) malloc(sizeof(list_t));
    newList->root = NULL;
    return newList;
}

node_t* createNode(int value){
    node_t* newNode = (node_t *) malloc(sizeof(node_t));
    newNode->value = value;
    newNode->next = NULL;
    return newNode;
}

void prependNode(list_t* newList, int value){
    node_t* newNode = createNode(value);
    newNode->next = newList->root;
    newList->root = newNode;
}

void freeList(list_t* list){
    node_t* currentNode = list->root;
    while (currentNode != NULL){
        node_t* nextNode = currentNode->next;
        free(currentNode);
        currentNode = nextNode;
    }
    
    free(list);
}
