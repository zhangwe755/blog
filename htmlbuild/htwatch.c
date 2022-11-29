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
    htDictPutNoFreeValue(htwatch.filedict, ht_watch_fd(fd), fileName);
    htDictPutNoFreeValue(htwatch.fddict, fileName, &fd);
    EV_SET(&(htwatch.tmpEvent), fd, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_CLEAR, NOTE_DELETE | NOTE_EXTEND | NOTE_RENAME | NOTE_WRITE | NOTE_ATTRIB | NOTE_LINK | NOTE_REVOKE, 0, 0);
    kevent(htwatch.kq, &(htwatch.tmpEvent), 1, NULL, 0, NULL);
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
        htDictRemove(htwatch.filedict, key, 0);
        return;
    }
    htDictRemove(htwatch.filedict, key, 0);
    htDictRemove(htwatch.fddict, fileName, 0);
    printf("File[%s] delete\n", fileName);
    if (htwatch.delete_handler != NULL) {
        htwatch.delete_handler(fileName);
    }
}

void ht_watch() {
    struct kevent events[1024];
    struct kevent event;
    int evcount;

    for (;;) {
        evcount = kevent(htwatch.kq, NULL, 0, events, 1024, NULL);
        if (evcount < 0) {
            printf("event while error!\n");
            continue;
        }
        for (int i=0; i<evcount; i++) {
            event = events[i];
            if (event.fflags & NOTE_DELETE) {
                ht_watch_handler_delete(event);
                continue;
            }
            if (event.fflags & NOTE_WRITE) {
                ht_watch_handler_delete(event);
                continue;
            }
            if (event.fflags & NOTE_ATTRIB)  {
                printf("File attributes modified\n");
            }
        }
    }
}
