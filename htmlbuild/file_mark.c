/**
 * parse the mark type 
 */
#include <string.h>
#include <stdlib.h>

#include "file_mark.h"
#include "util.h"
#include "list.h"

int same_char_num(htcharnode *indexNode) {
    htcharnode *curNode= indexNode;
    int sNum = 1;
    char sChar = curNode->data;
    while(curNode->nextNode != NULL) {
        if (curNode->data != sChar) {
            break;
        }
        sNum++;
        curNode = curNode->nextNode;
    }
    return sNum;
}

int is_new_line(htcharnode *curNode) {
    if ( curNode->preNode == NULL
            || curNode->preNode->data == '\r'
            || curNode->preNode->data == '\n') {
        return 1;
    }
    return 0;
}

htcharnode* code_end_node(htcharnode *curNode) {
    htcharnode *startNode = curNode->nextNode->nextNode->nextNode;
    if (startNode == NULL) {
        return NULL;
    }
    htcharnode *stopNode = NULL;
    while(startNode != NULL) {
        if ((char)startNode->data == '`' && same_char_num(startNode) == 3) {
            stopNode = startNode;
            break;
        }
        startNode = startNode->nextNode;
    }
    return stopNode;
}

htcharnode* next_line_end_pre_node(htcharnode *startNode) {
    htcharnode* cur= startNode;
    while(cur->nextNode!=NULL
            && cur->nextNode->data!='\r'
            && cur->nextNode->data!='\n') {
        cur = cur->nextNode;
    }
    return cur;
}

/**
 * return next mark flag may be start node
 */
htcharnode* mark_add_str(htcharlist *list, char* str, htcharnode *startNode, int removeCount) {
    htcharnode *ref = NULL;
    int curChar = 0;
    if (startNode == NULL) {
        ref = list->end;
    } else if(startNode == list->head) {
        removeCount--;
        startNode->data = str[0];
        ref = startNode;
        curChar = 1;
    } else {
        ref = startNode->preNode;
    }
    log_debug("mark_add_str str:%s", str);
    log_debug("mark_add_str end node next:%s, removeCount:%d", list->end->nextNode, removeCount);
    for (int i=0; i<removeCount; i++) {
        htRemoveCharNode(list, ref->nextNode);
        log_debug("mark_add_str end next:%s", list->end->nextNode);
    }
    log_debug("mark_add_str end:%c", list->end->data);
    log_debug("mark_add_str ref:%c", ref->data);
    log_debug("mark_add_str end next:%s", list->end->nextNode);
    for(int i=curChar;i<strlen(str); i++) {
        log_debug("\n");
        log_debug("mark_add_str add str[i]:%c", str[i]);
        htAddCharAfterRefUseData(list, str[i], ref);
        log_debug("mark_add_str end:%c", list->end->data);
        log_debug("mark_add_str ref:%c", ref->data);
        log_debug("mark_add_str end next:%s", list->end->nextNode);
        log_debug("mark_add_str ref next:%c", ref->nextNode->data);
        ref = ref->nextNode;
        log_debug("mark_add_str ref:%c", ref->data);
        log_debug("mark_add_str ref next:%s", ref->nextNode);
        log_debug("\n");
    }
    log_debug("mark_add_str add finish:%s", str);
    return ref->nextNode;
}

/**
 * Build mark and return new indexNode, the new indexNode is while index
 * call one time, build one mark flag
 */
htcharnode* try_build_mark(htcharlist *list, htcharnode *indexNode) {
    log_debug("try_build_mark: char:%c", (char)indexNode->data);
    htcharnode *cur = indexNode;
    if ((char)indexNode->data == '*') {
        log_debug("try_build_mark *");
        int preCount = 1;
        int endCount = 0;
        int preEnd = 0;
        htcharnode *endStartNode = NULL;
        while(cur!=NULL) {
            cur = cur->nextNode;
            if (cur != NULL && (char)cur->data == '*') {
                if (preEnd == 0) {
                    preCount++;
                } else {
                    if (endStartNode == NULL) {
                        endStartNode = cur;
                    }
                    endCount++;
                    if (endCount == 3) {
                        break;
                    }
                }
            } else {
                if (preEnd == 0) {
                    preEnd = 1;
                }
                if (endStartNode != NULL) {
                    break;
                }
            }
        }
        log_debug("pre count:%d", preCount);
        log_debug("end count:%d", endCount);
        cur = indexNode;
        if (endCount > 0) {
            if (preCount>endCount) {
                for (int i=0; i<preCount-endCount; i++) {
                    cur = cur->nextNode;
                }
                preCount = endCount;
            } else {
                endCount = preCount;
            }
            char *pre = NULL;
            char *end;
            if (preCount==1) {
                pre= "<div class=\"mk_italic\">";
                end= "</div>";
            }
            if (preCount==2) {
                pre= "<div class=\"mk_bold\">";
                end= "</div>";
            }
            if (preCount==3) {
                pre= "<div class=\"mk_bold_italic\">";
                end= "</div>";
            }
            htcharnode *maybeNode = NULL;
            if (pre != NULL) {
                maybeNode = mark_add_str(list, pre, cur, preCount);
                mark_add_str(list, end, endStartNode, preCount);
            }
            return maybeNode;
        }
    }
    if ((char)indexNode->data == '#' && is_new_line(indexNode)) {
        int level = 1;
        htcharnode *cur = indexNode;
        for (;level<6;) {
            cur = cur->nextNode;
            if (cur == NULL || (char)cur->data != '#') {
                break;
            }
            level++;
        }
        log_debug("#level:%d", level);
        htcharnode *preEndNode = next_line_end_pre_node(indexNode);
        log_debug("#pre end char:%c", preEndNode->data);
        char *pre = NULL;
        char *end;
        if (level == 1) {
            pre = "<h1>";
            end = "</h1>";
        }
        if (level == 2) {
            pre = "<h2>";
            end = "</h2";
        }
        if (level == 3) {
            pre = "<h3>";
            end = "</h3>";
        }
        if (level == 4) {
            pre = "<h4>";
            end = "</h4>";
        }
        if (level == 5) {
            pre = "<h5>";
            end = "</h5>";
        }
        if (level == 6) {
            pre = "<h6>";
            end = "</h6>";
        }
        cur = indexNode;
        if (pre != NULL) {
            mark_add_str(list, pre, cur, level);
            mark_add_str(list, end, preEndNode->nextNode, 0);
        }
        log_debug("#pre end char:%c", preEndNode->data);
        return preEndNode->nextNode;
    }
    if ((char)indexNode->data == '>' && is_new_line(indexNode)) {
        htcharnode *preEndNode = next_line_end_pre_node(indexNode);
        char *pre = "<div class='mk_ref'>";
        char *end = "</div>";
        cur = indexNode;
        if (pre != NULL) {
            mark_add_str(list, pre, cur, 1);
            mark_add_str(list, end, preEndNode->nextNode, 0);
        }
        log_debug("#pre end char:%c", preEndNode->data);
        return preEndNode->nextNode;
    }
    if ((char)indexNode->data == '`' && same_char_num(indexNode) == 3) {
        htcharnode *codeEndNode = code_end_node(indexNode);
        if (codeEndNode != NULL) {
            char *pre = "<pre class=\"mk_code\">";
            char *end = "</pre>";
            log_debug("pre codeEndNode:%c", codeEndNode->data);
            mark_add_str(list, pre, indexNode, 3);
            mark_add_str(list, end, codeEndNode, 3);
            return codeEndNode->nextNode->nextNode->nextNode;
        }
    }
    if ((char)indexNode->data == '\n') {
    }
    if ((char)indexNode->data == '(') {
    }
    if ((char)indexNode->data == '!') {
    }
    log_debug("build_mark_str indexNode:%s", indexNode);
    log_debug("build_mark_str indexNode-->:%c", indexNode->data);
    return indexNode->nextNode;
}

char* build_mark_str(char *src) {
    log_debug("src:%s",src);
    htcharlist *list = htCreateCharList();
    for (int i=0; i<strlen(src); i++) {
        htAddCharNodeUseData(list, src[i]);
    }

    htcharnode *indexNode = list->head;
    while (indexNode != NULL) {
        indexNode = try_build_mark(list, indexNode);
    }
    log_debug("build_mark_str list is ok!");
    log_debug("build_mark_str indexNode:%s", indexNode);
    char *destStr = malloc((list->len+1)*(sizeof(char)));
    indexNode = list->head;
    for (int i=0;i<list->len;i++) {
        destStr[i] = (char)indexNode->data;
        indexNode=indexNode->nextNode;
    }
    destStr[list->len] = '\0';
    htDestoryCharList(list);
    return destStr;
}

void build_mark(buildcontext *dest) {
    // parse mark and put in the dest
    log_info("build_mark=>build mark file:%s", dest->cur_file);

   // regist file in order to find root file
    registFile(dest->cur_file, dest->src_file);

    char *mark_src = ht_read_file(dest->cur_file);
    log_debug("build_mark src str:%s", mark_src);
    if (mark_src == NULL) {
        log_error("build_mark==>The file %s is empty!");
        return;
    }
    char *destStr = build_mark_str(mark_src);
    htFree(mark_src);
    log_debug("build_mark src str:%s", destStr);
    appendDestLine(dest->retList, destStr);
}
