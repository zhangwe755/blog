#include <stdio.h>

#include "htwatch.h"
#include "config.h"


int isRootFile(char *file) {
    return htDirIsParentDir(htconfig.root_dir, file);
}

void boot_update_file(char *fileName) {
    printf("boot update file:%s\n", fileName);
}

void boot_init(int argc, char **argv) {
    printf("start html builder!\n");
    // 加载配置
    ht_config_init(argc, argv);

    ht_watch_init();
    htwatch.update_handler = boot_update_file;

    // 遍历根目录,所有根文件先编译
    char *rootFile = "/Users/apple/soft/blog/templete/src";

    // 遍历所有子文件监听这些文件
    ht_watch_join(rootFile);

    ht_watch();
    printf("close html builder!\n");
}
