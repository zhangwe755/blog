#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "util.h"

void htCleanList(htlist *list) {
    list->head = NULL;
    list->end = NULL;
    list->len = 0;
}

htlist * htCreateList() {
    htlist *list = (htlist *)malloc(sizeof(htlist));
    htCleanList(list);
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

void htAddAfterRef(htlist *baseList, htnode *add, htnode *ref) {
    if (baseList->len == 0) {
        log_error("htAddAfterNode list len is 0, *ref not in baseList");
        exit(0);
    }
    if (ref == baseList->end) {
        htAddNode(baseList, add);
        return;
    }
    htnode *item = baseList->head;
    while(item != NULL) {
        if (item == ref) {
            break;
        }
        item = item->nextNode;
    }
    if (item == NULL) {
        log_error("htAddAfterNode list len is %d, *ref not in baseList", baseList->len);
        exit(0);
    }
    htnode *nextNode = ref->nextNode;
    ref->nextNode=add;
    add->preNode=ref;
    add->nextNode=nextNode;
    nextNode->preNode=add;
    baseList->len++;
}

void htAddAfterRefUseData(htlist *baseList, void *addData, htnode *ref) {
    htnode *node = malloc(sizeof(htnode));
    node->data = addData;
    node->preNode = NULL;
    node->nextNode = NULL;
    htAddAfterRef(baseList, node, ref);
}

void htRemoveNode(htlist *baseList, htnode *node) {
    if (baseList->head == node) {
        if (baseList->len == 1) {
            baseList->head = NULL;
            baseList->end = NULL;
        } else {
            htnode *next = baseList->head->nextNode;
            baseList->head = next;
            next->preNode = NULL;
        }
    } else if ( baseList->end == node ) {
        htnode *pre = baseList->end->preNode;
        baseList->end = pre;
        pre->nextNode = NULL;
    } else {
        node->preNode->nextNode = node->nextNode;
        node->nextNode->preNode = node->preNode;
    }
    free(node);
    baseList->len--;
}

void htDestoryList(htlist *baseList) {
    htnode *cur = baseList->head;
    htnode *killNode;
    while(cur != NULL) {
        killNode = cur;
        cur = cur->nextNode;
        free(killNode);
    }
    free(baseList);
}

/********** char list *************/

void htCleanCharList(htcharlist *list) {
    list->head = NULL;
    list->end = NULL;
    list->len = 0;
}

htcharlist * htCreateCharList() {
    htcharlist *list = (htcharlist *)malloc(sizeof(htcharlist));
    htCleanCharList(list);
    return list;
}

void htAddCharNode(htcharlist *baseList, htcharnode *node) {
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

void htAddCharNodeUseData(htcharlist *baseList, char data) {
    htcharnode *node = malloc(sizeof(htcharnode));
    node->data = data;
    node->preNode = NULL;
    node->nextNode = NULL;
    htAddCharNode(baseList, node);
}

void htAddCharAfterRef(htcharlist *baseList, htcharnode *add, htcharnode *ref) {
    log_debug("add: %c", add->data);
    if (baseList->len == 0) {
        log_error("htAddCharAfterNode list len is 0, *ref not in baseList");
        exit(0);
    }
    log_debug("========>add: %c", add->data);
    if (ref == baseList->end) {
        log_debug("========>ref is end");
        htAddCharNode(baseList, add);
        return;
    }
    log_debug("add: %c", add->data);
    htcharnode *item = baseList->head;
    while(item != NULL) {
        if (item == ref) {
            break;
        }
        item = item->nextNode;
    }
    if (item == NULL) {
        log_error("htAddCharAfterNode list len is %d, *ref not in baseList", baseList->len);
        exit(0);
    }
    log_debug("add: %c", add->data);
    htcharnode *nextNode = ref->nextNode;
    ref->nextNode=add;
    add->preNode=ref;
    add->nextNode=nextNode;
    nextNode->preNode=add;
    baseList->len++;
}

void htAddCharAfterRefUseData(htcharlist *baseList, char addData, htcharnode *ref) {
    htcharnode *node = malloc(sizeof(htcharnode));
    node->data = addData;
    node->preNode = NULL;
    node->nextNode = NULL;
    htAddCharAfterRef(baseList, node, ref);
}

void htRemoveCharNode(htcharlist *baseList, htcharnode *node) {
    if (baseList->head == node) {
        if (baseList->len == 1) {
            baseList->head = NULL;
            baseList->end = NULL;
        } else {
            htcharnode *next = baseList->head->nextNode;
            baseList->head = next;
            next->preNode = NULL;
        }
    } else if ( baseList->end == node ) {
        htcharnode *pre = baseList->end->preNode;
        baseList->end = pre;
        pre->nextNode = NULL;
    } else {
        node->preNode->nextNode = node->nextNode;
        node->nextNode->preNode = node->preNode;
    }
    free(node);
    baseList->len--;
}

void htDestoryCharList(htcharlist *baseList) {
    htcharnode *cur = baseList->head;
    htcharnode *killNode;
    while(cur != NULL) {
        killNode = cur;
        cur = cur->nextNode;
        free(killNode);
    }
    free(baseList);
}
