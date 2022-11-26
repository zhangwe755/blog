#include <stdio.h>

#include "htwatch.h"
#include "parse.h"

void te_update(char *fileName) {
    printf("te update %s\n", fileName);
    htlist *rootFileList = parseGetRootFile(fileName);
    if (rootFileList == NULL) {
        printf("========> no root file, childfile: %s, rootFileList is null\n", fileName);
        return;
    }
    if (rootFileList->len == 0 ) {
        printf("========> no root file, childfile: %s, rootFileList is size=0\n", fileName);
        return;
    }
    char * rootFileArray[rootFileList->len];
    htnode *tmp = rootFileList->head;
    int i = 0;
    while(tmp != NULL) {
        rootFileArray[i] = tmp->data;
        i++;
        tmp=tmp->nextNode;
    }

    char *rootFile;
    int len = rootFileList->len;
    for (i=0;i<len;i++) {
        rootFile = rootFileArray[i];
        buildRootFile(rootFile);
    }
}

int main() {
    char *rootFile = "/Users/apple/soft/blog/templete/src/rootdir/index.btm";
    buildRootFile(rootFile);

    ht_watch_init();
    htwatch.update_handler =  te_update;
    ht_watch_join(rootFile);
    ht_watch();

}

