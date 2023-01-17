#include <string.h>
#include "test_util.h"
#include "util.h"

void test_ht_read_file() {
    char *file_content = ht_read_file("./test.c");
    log_debug("file_ccontent size:%d", strlen(file_content));
}
