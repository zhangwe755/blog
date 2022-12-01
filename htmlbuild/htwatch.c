#include <stdio.h>
#include <sys/event.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "htwatch.h"
#include "util.h"

char *ht_watch_fd(long fd) {
    char *tem = malloc(12);
    sprintf(tem, "_%lu", fd); 
    return tem;
}

void ht_watch_file_regist(long fp, char *fileName) {
    htwatchfile *wf = malloc(sizeof(htwatchfile));
    wf->file = fileName;
    wf->mtime = fileUpdateTime(fileName);
    wf->fp = fp;
    htDictPutNoFreeValue(htwatch.filedict, ht_watch_fd(fd), wf);
    htDictPutNoFreeValue(htwatch.fddict, fileName, wf);
}

void ht_watch_file_unregist(htwatchfile *file) {
    htDictRemove(htwatch.filedict, ht_watch_fd(file->fd),  0);
    htDictRemove(htwatch.fddict, file->file, 0);
    free(file);
}

htwatchfile *ht_watch_file_from_fp(long fp) {
    return (htwatchfile *)htDictGet(htwatch.filedict, ht_watch_fd(fp));
}

int ht_watch_change_check_and_update(htwatchfile *file) {
    long nft = fileUpdateTime(file->file);
    if (file->mtime == ntf) {
        return 0;
    }
    file->mtime = ntf;
    return 1;
}

/**
 * watch init
 */
void ht_watch_init() {
    htwatch.filedict = htDictCreate(1024);
    htwatch.fddict = htDictCreate(1024);

    htwatch.kq = kqueue();
    if (htwatch.kq == -1) {
        printf("kqueue init error!\n");
        return;
    }
    htwatch.update_handler = NULL;
    htwatch.delete_handler = NULL;
}

int ht_watch_getfd(char *fileName) {
    int *fd =  htDictGet(htwatch.fddict, fileName);
    if (fd == NULL) {
        return -1;
    }
    return *fd;
}

void ht_watch_join(char *fileName) {
    long fd = open(fileName, O_RDONLY);
    if (fd == -1) {
        printf("open file error, file:%s\n", fileName);
        return;
    }
    EV_SET(&(htwatch.tmpEvent), fd, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_CLEAR, NOTE_DELETE | NOTE_EXTEND | NOTE_RENAME | NOTE_WRITE | NOTE_ATTRIB | NOTE_LINK | NOTE_REVOKE, 0, 0);
    kevent(htwatch.kq, &(htwatch.tmpEvent), 1, NULL, 0, NULL);
    ht_watch_file_regist(fd, fileName);
}    

void ht_watch_unjoin(htwatchfile *file) {
    //EV_SET(&(htwatch.tmpEvent), fd, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_CLEAR, NOTE_DELETE | NOTE_EXTEND | NOTE_RENAME | NOTE_WRITE | NOTE_ATTRIB | NOTE_LINK | NOTE_REVOKE, 0, 0);
    //kevent(htwatch.kq, &(htwatch.tmpEvent), 1, NULL, 0, NULL);
    ht_watch_file_unregist(file);
}

void ht_watch_handler_edit(struct kevent event) {
    char *key = ht_watch_fd(event.ident);
    char *fileName = (char *)htDictGet(htwatch.filedict, key);
    printf("File[%s] modified\n", fileName);
    if (htwatch.update_handler != NULL) {
        htwatch.update_handler(fileName);
    }
}

void ht_watch_handler_delete(struct kevent event) {
    printf("flags:%u , %d, %d\n", event.fflags, EV_ADD, EV_DELETE);
    char *key = ht_watch_fd(event.ident);
    char *fileName = (char *)htDictGet(htwatch.filedict, key);
    if (isFile(fileName)) {
        // 修改文件, 大部分编辑器修改文件是删除替换操作
        ht_watch_join(fileName);
        ht_watch_handler_edit(event);
        //htDictRemove(htwatch.filedict, key, 0);
        return;
    }
    //htDictRemove(htwatch.filedict, key, 0);
    //htDictRemove(htwatch.fddict, fileName, 0);
    printf("File[%s] delete\n", fileName);
    if (htwatch.delete_handler != NULL) {
        htwatch.delete_handler(fileName);
    }
}

void ht_watch() {
    struct kevent events[1024];
    struct kevent event;
    int evcount;
    int tmp_delete;
    char *tmp_file;

    htwatchevent wevents[1024];
    for (int i=0;i<1024;i++) {
        wevents[i].old = 1;
        wevents[i].dirpath = NULL;
        wevents[i].exist = 0;
    }

    for (;;) {
        int wvindex = 0;
        evcount = kevent(htwatch.kq, NULL, 0, events, 1024, NULL);
        if (evcount < 0) {
            printf("watch==> event while error!\n");
            continue;
        }
        printf("watch==> event count:%d\n", evcount);
        for (int i=0; i<evcount; i++) {
            tmp_delete = 0;
            event = events[i];
            htwatchfile *file = ht_watch_file_from_fp(event.ident);
            if (file == NULL) {
                printf("watch :event.ident not regist");
                continue;
            }
            if (!ht_watch_change_check_and_update(file)) {
                continue;
            }
            tmp_file = file->file;
            if (event.fflags & NOTE_DELETE) {
                ht_watch_unjoin(file);
            }
            // 检查父目录是否已经存在
        }
        // 遍历文件夹确定增加文件,确定变更文件 执行update,delete,add, 也有可能没有变更
    }
}
