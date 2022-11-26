#include <stdio.h>

#include "htwatch.h"

void te_update(char *fileName) {
    printf("te update %s\n", fileName);
}

int main() {
    char *rootFile = "/Users/apple/soft/blog/templete/src/rootdir/index.btm";
    ht_watch_init();
    htwatch.update_handler =  te_update;
    ht_watch_join(rootFile);
    ht_watch();

}

