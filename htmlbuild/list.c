#include <stdio.h>
#include <stdlib.h>

#include "list.h"


htlist * htCreateList() {
    htlist *list = (htlist *)malloc(sizeof(htlist));
    list->head = NULL;
    list->end = NULL;
    list->len = 0;
    return list;
}

void htAddNode(htlist *baseList, htnode *node) {
    if (baseList->len == 0) {
        baseList->head = node;
        baseList->end = node;
        baseList->len ++;
        return;
    }
    baseList->end->nextNode = node;
    node->preNode = baseList->end;
    baseList->end = node;
    baseList->len ++;
}

void htAddNodeUseData(htlist *baseList, void *data) {
    htnode *node = malloc(sizeof(htnode));
    node->data = data;
    node->preNode = NULL;
    node->nextNode = NULL;
    htAddNode(baseList, node);
}


void htRemoveNode(htlist *baseList, htnode *node) {
    if (baseList->head == node) {
        if (baseList->len == 1) {
            baseList->head = NULL;
            baseList->end = NULL;
        } else {
            htnode *next = baseList->head->nextNode;
            baseList->head = next;
        }
    } else if ( baseList->end == node ) {
        htnode *pre = baseList->end->preNode;
        baseList->end = pre;
    } else {
        node->preNode->nextNode = node->nextNode;
        node->nextNode->preNode = node->preNode;
    }
    free(node);
    baseList->len--;
}

