#ifndef HT_LIST
#define HT_LIST


typedef struct ht_node {
    struct ht_node *preNode;
    struct ht_node *nextNode;
    void *data;
} htnode;

typedef struct ht_list {
    htnode *head;
    htnode *end;
    int len;
    void *data;
} htlist;

#endif

htlist * htCreateList();
void htAddNode(htlist *baseList, htnode *node);
void htAddNodeUseData(htlist *baseList, void *data);
void htRemoveNode(htlist *baseList, htnode *node);

void htCleanList(htlist *baseList);



