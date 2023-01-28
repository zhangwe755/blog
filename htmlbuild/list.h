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
} htlist;

typedef struct ht_char_node {
    struct ht_char_node *preNode;
    struct ht_char_node *nextNode;
    char data;
} htcharnode;

typedef struct ht_char_list {
    htcharnode *head;
    htcharnode *end;
    int len;
} htcharlist;

#endif

htlist * htCreateList();
void htAddNode(htlist *baseList, htnode *node);
void htAddNodeUseData(htlist *baseList, void *data);
void htAddAfterRef(htlist *baseList, htnode *add, htnode *ref);
void htAddAfterRefUseData(htlist *baseList, void *addData, htnode *ref);
void htRemoveNode(htlist *baseList, htnode *node);
void htCleanList(htlist *baseList);
void htDestoryList(htlist *baseList);

htcharlist * htCreateCharList();
void htAddCharNode(htcharlist *baseList, htcharnode *node);
void htAddCharNodeUseData(htcharlist *baseList, char data);
void htAddCharAfterRef(htcharlist *baseList, htcharnode *add, htcharnode *ref);
void htAddCharAfterRefUseData(htcharlist *baseList, char addData, htcharnode *ref);
void htRemoveCharNode(htcharlist *baseList, htcharnode *node);
void htCleanCharList(htcharlist *baseList);
void htDestoryCharList(htcharlist *baseList);



