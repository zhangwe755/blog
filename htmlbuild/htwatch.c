#include <stdio.h>
#include <sys/event.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
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
    htDictPutNoFreeValue(htwatch.filedict, ht_watch_fd(fp), wf);
    htDictPutNoFreeValue(htwatch.fddict, fileName, wf);
}

void ht_watch_file_unregist(htwatchfile *file) {
    htDictRemove(htwatch.filedict, ht_watch_fd(file->fp),  0);
    htDictRemove(htwatch.fddict, file->file, 0);
    free(file);
}

htwatchfile *ht_watch_file_from_fp(long fp) {
    return (htwatchfile *)htDictGet(htwatch.filedict, ht_watch_fd(fp));
}

htwatchfile *ht_watch_file_from_file(char *file) {
    return (htwatchfile *)htDictGet(htwatch.fddict, file);
}

int ht_watch_change_check(htwatchfile *file) {
    long nft = fileUpdateTime(file->file);
    if (file->mtime == nft) {
        return 0;
    }
    return 1;
}

int ht_watch_change_check_and_update(htwatchfile *file) {
    long nft = fileUpdateTime(file->file);
    if (file->mtime == nft) {
        return 0;
    }
    file->mtime = nft;
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
    log_info("join watch, file:%s", fileName);
    htwatchfile *file = ht_watch_file_from_file(fileName);
    if (file != NULL) {
        return;
    }
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

void ht_watch_event_handler(htwatchevent wevents[], int count) {

    htwatchevent hevent;
    htwatchevent heventchild;
    char *filePath;
    for (int i=0;i<count;i++) {
        hevent = wevents[i];
        if (hevent.old) {
            break;
        }
        if (!isDir(hevent.dirpath)) {
            continue;
        }
        // 文件夹变更，子文件所有都会遍历，所以提前把单独的子文件去掉
        for (int j=0;i<count;j++) {
            heventchild = wevents[j];
            if (heventchild.old) {
                break;
            }
            if (i==j) {
                continue;
            }
            if (heventchild.dirpath == NULL) {
                continue;
            }
            if (isDirChild(hevent.dirpath, heventchild.dirpath)) {
                heventchild.dirpath = NULL;
            }
        }

    }
    for (int i=0;i<count;i++) {
        hevent = wevents[i];
        if (hevent.old) {
            break;
        }
        if (hevent.dirpath == NULL) {
            continue;
        }
        if (isFile(hevent.dirpath)) {
            ht_watch_join(hevent.dirpath);
            if (htwatch.update_handler != NULL) {
                htwatch.update_handler(hevent.dirpath);
            }
            continue;
        }
        if (isDir(hevent.dirpath)) {
            htlist *childlist = htCreateList();
            htdirchildren(childlist, hevent.dirpath);
            htnode *tmpNode = childlist->head;
            while(tmpNode != NULL) {
                filePath = tmpNode->data;
                if (isFile(filePath)) {
                    htwatchfile *file = ht_watch_file_from_file(filePath);
                    int update = 0;
                    if (file == NULL) {
                        update = 1;
                    } else {
                        update = ht_watch_change_check_and_update(file); 
                    }
                    if (update && htwatch.update_handler != NULL) {
                        htwatch.update_handler(filePath);
                    }
                }
                ht_watch_join(filePath);
                tmpNode = tmpNode->nextNode;
            }
        }
    }
}

void ht_watch_event_init(htwatchevent wevents[], int count) {
    for (int i=0;i<count;i++) {
        wevents[i].old = 1;
        wevents[i].dirpath = NULL;
    }
}

void ht_watch() {
    log_info("start watching!.......");
    struct kevent events[1024];
    struct kevent event;
    htwatchevent wevents[1024];
    int evcount;
    char *tmp_file;
    htwatchevent hevent;
    int has_add = 0;

    for (;;) {
        int wvindex = 0;
        ht_watch_event_init(wevents, 1024);
        evcount = kevent(htwatch.kq, NULL, 0, events, 1024, NULL);
        if (evcount < 0) {
            continue;
        }
        for (int i=0; i<evcount; i++) {
            has_add = 0;
            event = events[i];
            if (!(event.fflags & NOTE_DELETE) && !(event.fflags & NOTE_WRITE)) {
                continue;
            }
            htwatchfile *file = ht_watch_file_from_fp(event.ident);
            if (file == NULL) {
                printf("watch :event.ident not regist");
                continue;
            }
            tmp_file = file->file;
            if (isDir(tmp_file)) {
                if (!ht_watch_change_check_and_update(file)) {
                    continue;
                }
            } else {
                if (!ht_watch_change_check(file)) {
                    continue;
                }
            }
            if (event.fflags & NOTE_DELETE) {
                ht_watch_unjoin(file);
            }
            for (int j=0;j<1024;j++) {
                hevent = wevents[j];
                if (hevent.old) {
                    break;
                }
                if (strcmp(tmp_file, hevent.dirpath)==0) {
                    has_add = 1;
                    break;
                }
            }
            if (!has_add) { 
                wevents[i].old = 0;
                wevents[i].dirpath = tmp_file;
                i++;
            }
        }
        ht_watch_event_handler(wevents, 1024);
        // 遍历文件夹确定增加文件,确定变更文件 执行update,delete,add, 也有可能没有变更
    }
}
