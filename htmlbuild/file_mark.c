/**
 * parse the mark type 
 */
#include <string.h>
#include <stdlib.h>

#include "file_mark.h"
#include "util.h"
#include "list.h"

typedef struct mark_img_milestone {
    htcharnode *textStart;
    htcharnode *textEnd;
    htcharnode *linkStart;
    htcharnode *linkEnd;
    htcharnode *styleStart;
    htcharnode *styleEnd;
} markimgmil;

int same_char_num(htcharnode *indexNode) {
    htcharnode *curNode= indexNode;
    int sNum = 1;
    char sChar = curNode->data;
    while(curNode->nextNode != NULL) {
        if (curNode->nextNode->data != sChar) {
            break;
        }
        sNum++;
        curNode = curNode->nextNode;
    }
    return sNum;
}

int is_link(htcharnode *indexNode) {
    // 特征数量, 超链接有3个特征'['开头,']'后面一个字符是'(', ')'符号结尾
    int spe = 1;
    htcharnode *curNode =  indexNode;
    if (curNode->data != '[') {
        return 0;
    }
    while(curNode != NULL 
            && curNode->data != '\r'
            && curNode->data != '\n') {
        if (spe == 1 
                && curNode->data == ']'
                && curNode->nextNode != NULL
                && curNode->nextNode->data == '(') {
            spe = 2;
        }
        if (spe == 2 
                && curNode->data == ')') {
            spe = 3;
            break;
        }
        curNode=curNode->nextNode;
    }
    if (spe == 3) {
        return 1;
    }
    return 0;
}

int is_img(htcharnode *curNode) {
    if (curNode->data != '!' || curNode->nextNode == NULL) {
        return 0;
    }
    return is_link(curNode->nextNode);
}

void reset_img_mil(markimgmil *mil, htcharnode *indexNode) {
    mil->textStart = NULL;
    mil->textEnd = NULL;
    mil->linkStart = NULL;
    mil->linkEnd = NULL;
    mil->styleStart = NULL;
    mil->styleEnd = NULL;
    htcharnode *curNode =  indexNode;
    while(curNode != NULL 
            && curNode->data != '\r'
            && curNode->data != '\n') {
        if (curNode->data == '[' && mil->textStart == NULL) {
            mil->textStart = curNode->nextNode;
        }
        if (curNode->data == ']' 
                && curNode->nextNode != NULL
                && curNode->nextNode->data == '(') {
            mil->textEnd = curNode->preNode;
        }
        if (curNode->data == '(' 
                && curNode->nextNode != NULL
                && curNode->preNode->data == ']') {
            mil->linkStart = curNode->nextNode;
        }
        if (curNode->data == ')' 
                && mil->linkStart != NULL
                && mil->linkEnd == NULL) {
            mil->linkEnd = curNode->preNode;
            if (curNode->nextNode != NULL
                    && curNode->nextNode->data == '(') {
                mil->styleStart = curNode->nextNode->nextNode;
                curNode=curNode->nextNode;
            } else {
                break;
            }
        }
        if (curNode->data == ')' 
                && mil->styleStart != NULL
                && mil->styleEnd == NULL) {
            mil->styleEnd = curNode->preNode;
            break;
        }
        curNode=curNode->nextNode;
    }
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
    } else {
        removeCount--;
        startNode->data = str[0];
        ref = startNode;
        curChar = 1;
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
    log_debug("mark_add_str add finish:%s, ref:%c", str, ref->data);
    return ref;
}

void mark_build_img(htcharlist *list, htcharnode *indexNode, char *str1, char *str2, char *str3, char*str4) {
    if (indexNode->data == '!') {
        mark_add_str(list, str1, indexNode, 2);

    }

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
            log_debug("codeEndNode:%c", codeEndNode->data);
            mark_add_str(list, pre, indexNode, 3);
            mark_add_str(list, end, codeEndNode, 3);
            log_debug("codeEndNode:%c", codeEndNode->data);
            return codeEndNode->nextNode->nextNode->nextNode->nextNode->nextNode;
        }
    }
    if (is_link(indexNode)) {
        // 超链接
        markimgmil *mil = malloc(sizeof(markimgmil));
        reset_img_mil(mil, indexNode);
        htCharExtra(list, mil->textStart, mil->textEnd);
        htCharExtra(list, mil->linkStart, mil->linkEnd);
        int removeCount = 4;
        if(mil->styleEnd != NULL) {
            removeCount = 6;
            htCharExtra(list, mil->styleStart, mil->styleEnd);
        }
        char *pre = "<a href=\"";
        char *mid = "\">";
        char *end = "</a>";
        htcharnode *curNode = indexNode;
        curNode = mark_add_str(list, pre, curNode, removeCount);
        // add link
        htAddSimpleListCharAfterRef(list, mil->linkStart, mil->linkEnd, curNode);
        curNode = mil->linkEnd->nextNode;
        curNode = mark_add_str(list, mid, curNode, 0);
        // add text
        htAddSimpleListCharAfterRef(list, mil->textStart, mil->textEnd, curNode);
        curNode = mil->textEnd->nextNode;
        curNode = mark_add_str(list, end, curNode, 0);
        free(mil);
        return curNode;
    }
    if (is_img(indexNode)) {
        // 图片链接
        markimgmil *mil = malloc(sizeof(markimgmil));
        reset_img_mil(mil, indexNode);
        htCharExtra(list, mil->textStart, mil->textEnd);
        htCharExtra(list, mil->linkStart, mil->linkEnd);
        int removeCount = 4;
        if(mil->styleEnd != NULL) {
            removeCount = 6;
            htCharExtra(list, mil->styleStart, mil->styleEnd);
        }
        char *pre = "<img src=\"";
        char *mid = "\" alt=\"";
        char *mid2 = "\" style=\"";
        char *end = "\"/>";
        htcharnode *curNode = indexNode;
        curNode = mark_add_str(list, pre, curNode, removeCount);
        // add link
        htAddSimpleListCharAfterRef(list, mil->linkStart, mil->linkEnd, curNode);
        curNode = mil->linkEnd->nextNode;
        curNode = mark_add_str(list, mid, curNode, 0);
        // add text
        htAddSimpleListCharAfterRef(list, mil->textStart, mil->textEnd, curNode);
        curNode = mil->textEnd->nextNode;
        curNode = mark_add_str(list, mid2, curNode, 0);
        if (mil->styleEnd != NULL) {
            htAddSimpleListCharAfterRef(list, mil->styleStart, mil->styleEnd, curNode);
            curNode = mil->styleEnd->nextNode;
        }
        curNode = mark_add_str(list, end, curNode, 0);
        free(mil);
        return curNode;
    }
    if ((char)indexNode->data == '\n') {
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
