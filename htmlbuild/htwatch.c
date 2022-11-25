#include <stdio.h>
#include <sys/event.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "htwatch.h"


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
}

void ht_watch_join(char *fileName) {
    int fd = open(fileName, O_RDONLY);
    if (fd == -1) {
        printf("open file error, file:%s\n", fileName);
        return;
    }
    char *tem = malloc(12);
    sprintf(tem, "_%d", fd); 
    htDictPutNoFreeValue(htwatch.filedict, tem, fileName);
    EV_SET(&(htwatch.tmpEvent), fd, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_CLEAR, NOTE_DELETE | NOTE_EXTEND | NOTE_RENAME | NOTE_WRITE | NOTE_ATTRIB | NOTE_LINK | NOTE_REVOKE, 0, 0);
    kevent(htwatch.kq, &(htwatch.tmpEvent), 1, NULL, 0, NULL);
}    

void ht_watch_handler_delete(struct kevent event) {
    char * data = (char *) event.udata;
    printf("File[%s] delete\n", data);
}

void ht_watch_handler_edit(struct kevent event) {
    char * data = (char *) event.udata;
    printf("File[%s] modified\n", data);
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
