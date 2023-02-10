#include "build.h"
#include "util.h"
#include "dict.h"

char *full_path(char *filePath) {
    return NULL;
}

char *cfrag_to_str(cfrag *frag) {
    return NULL;
}

root_context * ht_init_root_context(char *root_file, char *tigger) {
    return NULL;
}

fresult* ht_build_child(char *fpath) {
    fpath = full_path(fpath);
    /**
     * md5验证
     * 1 加载缓存文件
     * 2 解析文件
     *   2.1 解析文件
     *   2.2 更新缓存文件
     */
    return NULL;
}

void ht_build_root(rcontext *rc) {
    /**
     * md5验证
     * 1 加载缓存文件
     * 2 解析文件
     *   2.1 解析文件
     *   2.2 更新缓存文件
     */
}

void ht_build(char *rootFile, char *tigger) {
    rootFile = full_path(rootFile);
    tigger = full_path(tigger);
    rcontext * rc = ht_init_root_context(root_file, tigger);

    ht_build_root(rc);

    htnode *curNode = rc->sinc_frag_list->head;
    char* filePath = NULL;
    while(curNode != NULL) {
        filePath = cfrag_to_str(curNode->data);
        fresult* result = ht_build_child(filePath);
        if (result->error != NULL) {
            htDictPut(filePath, result);
        } else {
            log_error("file build fail! error:%s root_path:%s path:%s", rootFile, filePath, );
            return;
        }
    }

    if (rc->minc_frag != NULL) {
        filePath = cfrag_to_str(rc->minc_frag);
        filePath = full_path(filePath);
        // 获取所有子文件
        // 遍历所有子文件
        // 判断所有子文件是否是有效的文件
        // 填充变量文件
        // 生成目标文件
    } else {
        // 填充变量文件
        // 生成目标文件
    }
}
