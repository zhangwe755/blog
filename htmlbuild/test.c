#include <stdio.h>

#include "htwatch.h"

int main() {
    char *rootFile = "/Users/apple/soft/blog/templete/src/rootdir/index.btm";
    ht_watch_init();
    ht_watch_join(rootFile);
    ht_watch();

}

