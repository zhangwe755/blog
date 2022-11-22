#include <stdio.h>

#include "util.h"
#include "parse.h"
#include "list.h"

int main() {
    char *rootFile = "/Users/apple/soft/blog/templete/src/rootdir/index.btm";
    buildRootFile(rootFile);
    int a = 1 << 2;
    printf("a ==> %d\n", a & 1);
    printf("b ==> %d\n", a & 1<<2);
}

