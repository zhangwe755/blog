#include <stdio.h>

#include "htwatch.h"
#include "config.h"
#include "util.h"
#include "list.h"
#include "parse.h"


int isRootFile(char *file) {
    return htDirIsParentDir(htconfig.root_dir, file);
}

void boot_update_file(char *fileName) {
    if (isRootFile(fileName)) {
        buildRootFile(fileName);
        return;
    }
    buildChildFile(fileName);
}

void boot_build_rootfile() {
    log_info("start build root");
    htlist *fileList = htCreateList();
    htfilerecursivedetail(fileList, htconfig.watch_dir,1); 
    htnode *tmpNode = fileList->head;
    while(tmpNode != NULL) {
        buildRootFile(tmpNode->data);
        tmpNode = tmpNode->nextNode;
    }
    log_info("end build root");
}

void boot_init(int argc, char **argv) {
    log_info("start html builder!");
    // 加载配置
    ht_config_init(argc, argv);

    boot_build_rootfile();

    ht_watch_init();
    htwatch.update_handler = boot_update_file;

    htlist *fileList = htCreateList();
    htfilerecursivedetail(fileList, htconfig.watch_dir,0); 
    htnode *tmpNode = fileList->head;
    while(tmpNode != NULL) {
        ht_watch_join(tmpNode->data);
        tmpNode = tmpNode->nextNode;
    }

    ht_watch();
    log_info("close html builder!");
}
