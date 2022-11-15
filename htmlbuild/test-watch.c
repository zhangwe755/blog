// 配置读取
// 文件监听
// 网页模版文件
// markdown文件
// 本地简单服务器
// 自动更新浏览器
#include <sys/event.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char TEST_FILE[] = "/Users/apple/soft/test.text";
char TEST_FILE_2[] = "/Users/apple/soft/blog/htmlbuild/Makefile.xx";

void join_queue(char *fileName, int kq,struct kevent change) {
    int fd;
    fd = open(TEST_FILE, O_RDONLY);
    if (fd == -1) {
        printf("open file error, file:%s\n", fileName);
    }
    EV_SET(&change, fd, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_CLEAR, NOTE_DELETE | NOTE_EXTEND | NOTE_RENAME | NOTE_WRITE | NOTE_ATTRIB | NOTE_LINK | NOTE_REVOKE, 0, 0);
    kevent(kq, &change, 1, NULL, 0, NULL);
}

void watch() {
    int kq;
    // file descriptor for the file we are monitoring
    int fd,fd2;
    // new events that are returned from kevent
    int nev;
    // the new events that we want to the kernel to notifiy us about.
    struct kevent change;
    struct kevent change2;
    // the events that kevent is reporting. This gets filled by the kernel
    struct kevent event;
    struct kevent events[20];

    // ask the kernel to create a new queue. Will return file descriptor representing
    // the queue.
    kq = kqueue();
    if (kq == -1) {
        perror("kqueue");
    }
    join_queue(TEST_FILE, kq, change);

    for (;;) {
        nev = kevent(kq, NULL, 0, events, 20, NULL);
        if (nev == -1) {
            perror("kevent");
        } else if (nev > 0) {
            event = events[0];
            if (event.fflags & NOTE_DELETE) {
                join_queue(TEST_FILE, kq, change);
                printf("File deleted. Exiting\n");
            }
            if (event.fflags & NOTE_WRITE) {
                char * data = (char *) event.udata;
                printf("File[%s] modified\n", data);
            }
            if (event.fflags & NOTE_ATTRIB)  {
                printf("File attributes modified\n");
            }
        }
    }
    close(kq);
    close(fd);
    watch();
}

int main(int argc, char **argv) {
    watch();
    return EXIT_SUCCESS;
}

