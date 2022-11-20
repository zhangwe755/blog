#include <stdio.h>

#include "util.h"
#include "parse.h"
#include "list.h"

int main() {
    char *cmd_src = "@{xiaoming|dongdong|/Users/apple/soft/blog/htmlbuild|liuliu}";
    cmdentity *entity = parseCmdStr(cmd_src);
    htnode *head = entity->file_list->head;
    while (head != NULL) {
        printf("file :%s\n", head->data);
        head = head->nextNode;
    }
}

