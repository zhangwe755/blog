/**
 * parse the mark type 
 */
#include <string.h>
#include "file_mark.h"
#include "util.h"


void build_mark(buildcontext *dest) {
    // parse mark and put in the dest
    log_info("build_mark=>build mark file:%s", dest->cur_file);

   // regist file in order to find root file
    registFile(dest->cur_file, dest->src_file);

    char *mark_src = ht_read_file(dest->cur_file);
    int in_mark = 0;
    int mark_start = 0;
    int mark_end = 0;
    for (int i=0; i<strlen(mark_src); i++) {
    }
}
