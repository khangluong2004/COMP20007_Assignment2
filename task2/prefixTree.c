/*
    Written by Grady Fitzpatrick for 
    COMP20007 Assignment 2 2024 Semester 1
    
     Implementation for module which contains Prefix Trie 
        data structures and functions.
*/
#include "prefixTree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <string.h>

struct prefixTree *newPrefixTree(){
    /* IMPLEMENT HERE */
    /* Structure of the prefix tree includes a dummy node. */
    struct prefixTree *retTree = (struct prefixTree *) malloc(sizeof(struct prefixTree));

    // Initialize the children and visited field
    for (int i=0; i < CHILD_COUNT; i++){
        retTree->child[i] = NULL;
    }
    retTree->visited = UNMARKED;
    retTree->dictionaryIndex = -1;

    return retTree;
}

struct prefixTree *addWordToTree(struct prefixTree *pt, char *word, int dictionaryIndex){
    /* FILL IN: IMPLEMENT */
    struct prefixTree *current = pt;
    int position = 0;
    while (word[position] != '\0'){
        if (current->child[(int) word[position]] == NULL){
            current->child[(int) word[position]] = newPrefixTree();
        }
        current = current->child[(int) word[position]];
        position++;
    }

    // Add the '\0' for the final node if not already there
    if (current->child[(int) '\0'] == NULL){
        current->child[(int) '\0'] = newPrefixTree();
        // Put the dictionary index into the final node
        current->child[(int) '\0']->dictionaryIndex = dictionaryIndex;
    }

    return pt;
}

void freePrefixTree(struct prefixTree *pt){
    if (pt == NULL){
        return;
    }

    for (int i=0; i < CHILD_COUNT; i++){
        freePrefixTree(pt->child[i]);
    }

    free(pt);
    return;
}
