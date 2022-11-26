#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dict.h"

long _hashCode(char *hashStr) {
    long code = 0;
    for (int i=0; i<strlen(hashStr); i++) {
        char charindex = hashStr[i];
        code = (code << 5) + code + charindex;
    }
    return code < 0 ? -code : code;
}

htlist * _getSoltList(htdict *dict, char *key) {
    long hashCode=_hashCode(key);
    printf("hashCode %ld\n", hashCode);
    int solt = hashCode % dict->soltCount;
    printf("solt %d\n", solt);
    return dict->solts[solt];
}

htnode * _getEntryNode(htlist *entryList, char *key) {
    htnode *entryNode = NULL;
    htnode *tmpentryNode = entryList->head;
    printf("_getEntryNode len:%d\n", entryList->len);
    for (int i=0;i<entryList->len;i++) {
        htdictentry *entry = tmpentryNode->data;
        printf("_getEntryNode entry.key:%s, entry.value:%s\n", entry->key, entry->value);
        if(strcmp(entry->key, key) == 0) {
            entryNode = tmpentryNode;
            break;
        }
        tmpentryNode = tmpentryNode->nextNode;
    }
    return entryNode;

}

htdict * htDictCreate(int soltCount) {
    htdict *dict = (htdict *)malloc(sizeof(htdict) + soltCount * sizeof(htlist));
    dict->len=0;
    dict->soltCount=soltCount;
    for (int i=0;i<soltCount;i++) {
        dict->solts[i] = htCreateList();
    }
    return dict;
}

void htDictPut(htdict *dict, char *key, void *value, int freeOldValue) {
    htlist *list = _getSoltList(dict, key);
    printf("list len:%d\n", list->len);
    htnode *node = _getEntryNode(list, key);
    printf("get node\n");
    if (node != NULL) {
        printf("node not null\n");
        htdictentry *entry = node->data;
        printf("entry value:%s\n", entry->value);
        if (freeOldValue) {
            free(entry->value);
        }
        entry->value = value;
        return;
    }
    printf("no node\n");

    htdictentry *entry = (htdictentry *)malloc(sizeof(htdictentry));
    entry->key=key;
    entry->value=value;
    htAddNodeUseData(list, entry);
}

void htDictPutNoFreeValue(htdict *dict, char *key, void *value) {
    htDictPut(dict, key, value, 0);
}

void * htDictGet(htdict *dict, char *key) {
    htlist *list = _getSoltList(dict, key);
    htnode *node = _getEntryNode(list, key);
    printf("htDictGet get node, key:%s\n", key);
    if (node == NULL) {
        printf("htDictGet get node is null\n");
        return NULL;
    }
    htdictentry *entry = node->data;
    printf("htDictGet get node, key:%s\n", entry->key);
    return entry->value;
}

void htDictRemove(htdict *dict, char *key, int freeOldValue) {
    htlist *list = _getSoltList(dict, key);
    htnode *node = _getEntryNode(list, key);
    if (node == NULL) {
        return;
    }
    htRemoveNode(list, node);
    htdictentry *entry = node->data;
    if (freeOldValue) {
        free(entry->value);
        free(entry);
        free(node);
    }
}

void htDictRemoveNoFreeValue(htdict *dict, char *key) {
    htDictRemove(dict, key, 0);
}
