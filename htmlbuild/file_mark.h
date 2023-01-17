/**
 * parse the mark type source file
 * 语法分类:
 * 1、可以直接替换标记
 *    - *
 *    - **
 *    - ***
 *    - #
 *    - ##
 *    - ###
 *    - ####
 *    - #####
 *    - ######
 *    - 换行
 *    - ```
 *    - >
 * 2、需整体替换标记
 *    - (xxxx)[http://a.com]
 *    - !(xxxx)[http://a.com/test.png]
 */
#include "parse.h"

struct mark_item {
    int type;
    char *src_item;
    char *tag_item;
    int start;
    int end;
}markitem;

void build_mark(buildcontext *dest);
