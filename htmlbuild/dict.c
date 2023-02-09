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
    int solt = hashCode % dict->soltCount;
    return dict->solts[solt];
}

htnode * _getEntryNode(htlist *entryList, char *key) {
    htnode *entryNode = NULL;
    htnode *tmpentryNode = entryList->head;
    for (int i=0;i<entryList->len;i++) {
        htdictentry *entry = tmpentryNode->data;
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
    htnode *node = _getEntryNode(list, key);
    if (node != NULL) {
        htdictentry *entry = node->data;
        if (freeOldValue) {
            free(entry->value);
        }
        entry->value = value;
        return;
    }

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
    if (node == NULL) {
        return NULL;
    }
    htdictentry *entry = node->data;
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
        free(entry);
        free(node);
    }
}

void htDictRemoveNoFreeValue(htdict *dict, char *key) {
    htDictRemove(dict, key, 0);
}

hditerator* htDictStartIterator(htdict* dict) {
    hditerator *it = malloc(sizeof(hditerator));
    it->solt_id = -1;
    it->curNode = NULL;
    it->dict = dict;
    return it;
}

htdictentry* htDictIteratorNext(hditerator *it) {
    while(true) {
        if (it->curNode == NULL || it->curNode->nextNode == NULL) {
            it->solt_id++;
            if (it->dict->len >= it->solt_id) {
                break;
            }
            htlist *list = it->dict->solts[it->solt_id];
            if (list->len > 0) {
                it->curNode = list->head;
                return it->curNode->data;
            }
            it->curNode = NULL;
        } else {
            it->curNode = it->curNode->nextNode;
            return it->curNode->data;
        }
    }
    return NULL;
}
