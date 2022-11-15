#include <stdio.h>

#include "parse.h"

int main() {
    char *rootDir = "/Users/apple/soft/blog/templete/dest/rootdir/index.html";
    FILE * fp = deleteAndCreateFile(rootDir);
    fputs("hello world index.html!", fp);
    fputs("hello world index.html!", fp);
    fclose(fp);
    /*
    char *rootFile = "/Users/apple/soft/blog/templete/src/rootdir/index.btm";
    htlist *destList = htCreateList();
    printf("point 1!, list len:%d\n", destList->len);
    parseFile(destList, rootFile);
    printf("point 2!, list len:%d\n", destList->len);

    htnode *tmpNode = destList->head;
    printf("point 3!\n");
    for (int i=0;i < destList->len;i++) {
        printf("result is:%s", tmpNode->data);
        tmpNode = tmpNode->nextNode;
    }
    */
}
