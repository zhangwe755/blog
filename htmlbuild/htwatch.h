#include <sys/event.h>

#include "dict.h"

typedef struct ht_watch_context {

    struct kevent tmpEvent;
    int kq;

    htdict *filedict;

    htdict *fddict;
} htwatchcontext;

htwatchcontext htwatch;

void ht_watch_init();
void ht_watch_join(char *fileName);
void ht_watch();
