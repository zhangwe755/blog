#include <sys/event.h>

#include "dict.h"

#ifndef HT_WATCH
#define HT_WATCH


typedef struct ht_watch_event {
    int old;
    char *dirpath;
} htwatchevent;

typedef struct ht_watch_file {
    long mtime;
    char *file;
    long fp;
} htwatchfile;

typedef struct ht_watch_context {
    struct kevent tmpEvent;
    int kq;
    void (*delete_handler)(char*);
    void (*update_handler)(char*);
    htdict *filedict;
    htdict *fddict;
} htwatchcontext;

htwatchcontext htwatch;

#endif

void ht_watch_init();
void ht_watch_join(char *fileName);
void ht_watch();
