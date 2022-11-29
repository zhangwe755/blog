#include <stdio.h>

#include "htwatch.h"
#include "parse.h"
#include "config.h"

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

int main_main(int argc, char **argv) {
    // 解析所有配置
     

    ht_watch_init();
    htwatch.update_handler =  te_update;

    // 遍历根目录,所有根文件先编译
    char *rootFile = "/Users/apple/soft/blog/templete/src/rootdir/index.btm";
    buildRootFile(rootFile);

    // 遍历所有子文件监听这些文件
    ht_watch_join(rootFile);

    ht_watch();
    return 0;
}

int main_config(int argc, char **argv) {
    char *arguv[] = {"root_dir=/hello/info1","config_file=/Users/apple/soft/blog/templete/src/configfile", "dest_dir=/hello/info2", "ddd=/hello/info3"};
    ht_config_init(4, arguv);
    char *value = ht_config_get("root_dir");
    printf("config value ==>%s\n", value);
    printf("config value1==>%s\n", htconfig.root_dir);
    return 0;
} 

int main() {
    char *path= "/Users/apple/soft/blog/templete/src";
    ht_watch_init();
    ht_watch_join(path);
    ht_watch();
    return 0;
}
