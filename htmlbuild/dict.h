#include "list.h"

#ifndef HT_DICT
#define HT_DICT


typedef struct ht_dict_entry {
    char *key;
    void *value;
} htdictentry;

typedef struct ht_dict {
    long len;
    int soltCount;
    htlist *solts[];
} htdict;

typedef struct ht_dict_iterator {
    int solt_id;
    /**
     * 当前entryNode
     */
    htnode *curNode;
    htdict* dict;
}hditerator;

#endif

htdict * htDictCreate(int soltCount);
void htDictPut(htdict *dict, char *key, void *value, int freeOldValue);
void htDictPutNoFreeValue(htdict *dict, char *key, void *value);
void * htDictGet(htdict *dict, char *key);
void htDictRemove(htdict *dict, char *key, int freeOldValue);
void htDictRemoveNoFreeValue(htdict *dict, char *key);

/**
 * 生成一个迭代器
 */
hditerator* htDictStartIterator(htdict *dict);
/**
 * 用迭代器获取下一个元素
 * 最后一个元素返回NULL
 */
htdictentry* htDictIteratorNext(hditerator *it);


