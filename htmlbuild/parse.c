#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"


#define FILE_TYPE_HTML 1
#define FILE_TYPE_MARK 2

#define PATH_NORMAL 1
#define PATH_FUZZY_NAME 2

/**
 * 命令格式@{tttt:pathstr}
 * @{ 为特殊字符代表命令开始
 * tttt 限定文件类型:html mark. html代表普通html片段, mark代表文件是markdown类型
 * : 特殊字符串表示文件类型和文件路径直接的分隔符
 * pathstr 文件路径字符串
 *      1: 文件全路径
 *      2: 相对路径
 *      3: 模糊匹配 file*.md代表file开头,.md结尾的所有文件
 *      4: 文件夹格式 /path1/path2 其中path2代表的是文件夹时,命令匹配文件夹下的所有文件
 *      3: 枚举多个文件filepath1|filepath2|filepath3
 *          filepath1,filepath2,filepath3如果是文件夹或者模糊匹配时候代表文件夹下符合条件的文件
 */
cmdentity * parseCmdStr(char *cmdStr) {
    printf("src cmdStr:%s\n", cmdStr);
    int len = strlen(cmdStr);
    char tmp;
    int charIndex = 0;
    cmdentity *entity = malloc(sizeof(cmdentity)); 
    entity->src_cmd = cmdStr;
    entity->file_list = htCreateList();

    // TODO 解析文件路径

    if (strncmp("html", cmdStr + 2, (size_t)4) == 0) {
        printf("%s is html!\n", cmdStr);
        entity->file_type = FILE_TYPE_HTML;
        return entity;
    } else if (strncmp("mark", cmdStr + 2, (size_t)4) == 0) {
        printf("%s is mark!\n", cmdStr);
        entity->file_type = FILE_TYPE_MARK;
        return entity;
    }

    return NULL;
}

char * extraCmdStr(char *cmdStr, charindex *point) {
    return htSubstr(cmdStr+point->start, point->end - point->start + 1);
}

void appendDestLine(htlist *destList, char * destLine) {
    htAddNodeUseData(destList, destLine);
}

void parseHtml(htlist *destList, char *src, cmdentity *cmd) {
    printf("parseHtml!\n");
    printf("src %s, n:%d\n", src, cmd->point->start);
    // pre
    char *pre = htSubstr(src, cmd->point->start);
    printf("pre %s\n", pre);
   
    appendDestLine(destList, pre);
    printf("html path:%s\n", cmd->path);

    char *line = malloc(1024*100);
    int len = 0;
    FILE *fp = NULL;
    fp = fopen(cmd->path, "r");
    printf("html path:%s\n", cmd->path);
    do {
        line = fgets(line, 1024*100, fp);
        if (line != NULL) {
            // appendLine
            len = strlen(line);
            char *tmpLine = malloc(len+1);
            strncpy(tmpLine, line, len);
            tmpLine[len] = '\0';
            appendDestLine(destList, tmpLine);
            printf("append:%s\n", tmpLine);
        }
    } while(line != NULL);
    // end
    appendDestLine(destList, src + cmd->point->end + 1);
    free(line);
}

void parseFileV1(htlist *destList, char *rootFile) {
    char *line, *destLine;
    charindex *point;
    cmdentity *cmd;
    int len;
    line = malloc(1024*100);
    FILE *fp = NULL;
    fp = fopen(rootFile, "r");
    do {
        line = fgets(line, 1024*100, fp);
        if (line != NULL) {
            len = strlen(line);
            destLine = malloc(len+1);
            strncpy(destLine, line, len);
            destLine[len] = '\0';
            printf("line ==> %s\n", line);
            printf("destline ==> %s\n", destLine);
            
            // 解析文件，如果没有命令就直接写到临时文件
            point = searchCmdIndex(destLine);
            if (point == NULL) {
                // 原始字符串
                printf("point == null\n");
                appendDestLine(destList, destLine);
                continue;
            }
            cmd = parseCmdStr(extraCmdStr(destLine, point));
            if (cmd == NULL) {
                // 原始字符串
                printf("cmd == null\n");
                appendDestLine(destList, destLine);
                continue;
            }
            cmd->point = point;
            // 被识别的类型，需要解析
            if (cmd->file_type == FILE_TYPE_HTML) {
               parseHtml(destList, destLine, cmd);
            }
        }
    } while(line != NULL);
}

/**
 * 递归编译一个文件
 */
void parseFile(filedest *dest, char *filePath) {
    char *line, *destLine;
    charindex *point;
    cmdentity *cmd;
    htlist *middleFileList;
    int len;
    line = malloc(1024*100);
    FILE *fp = NULL;
    fp = fopen(rootFile, "r");
    do {
        line = fgets(line, 1024*100, fp);
        if (line != NULL) {
            len = strlen(line);
            destLine = malloc(len+1);
            strncpy(destLine, line, len);
            destLine[len] = '\0';
            printf("line ==> %s\n", line);
            printf("destline ==> %s\n", destLine);
            
            // 解析文件，如果没有命令就直接写到临时文件
            point = searchCmdIndex(destLine);
            if (point == NULL) {
                // 原始字符串
                printf("point == null\n");
                appendDestLine(dest->startList, destLine);
                continue;
            }
            cmd = parseCmdStr(extraCmdStr(destLine, point));
            if (cmd == NULL) {
                // 原始字符串
                printf("cmd == null\n");
                appendDestLine(dest->startList, destLine);
                continue;
            }
            cmd->point = point;
            // 被识别的类型，需要解析
            if (cmd->file_type == FILE_TYPE_HTML) {
               parseHtml(destList, destLine, cmd);
            }
        }
    } while(line != NULL);
}

charindex * searchCmdIndex(char *srcLine) {
    charindex * point = malloc(sizeof(charindex));
    int len = strlen(srcLine);
    int pre = 0, cmd = 0;
    char tmp;
    for (int i=0; i<len; i++) {
        tmp = srcLine[i];
        if (pre && cmd) {
            if (tmp == '}') {
                point->start = pre;
                point->end = i;
                return point;
            }
        } else if(pre && !cmd) {
            if (tmp == '{') {
                cmd = 1;
            } else {
                pre = 0;
            }
        } else {
            if (tmp == '@') {
                pre = i;
            }
        }
    }
    free(point);
    return NULL;
}

void buildDestFile(htlist *destList, char *destFile) {
    FILE *fp = deleteAndCreateFile(destFile);
    htnode *tmpNode = destList->head;
    for (int i=0;i < destList->len;i++) {
        fputs(tmpNode->data ,fp);
        tmpNode = tmpNode->nextNode;
    }
}

void buildFile(filedest *dest) {
    char *line, *destLine;
    charindex *point;
    cmdentity *cmd;
    htlist *middleFileList;
    int len;
    line = malloc(1024*100);
    FILE *fp = fopen(dest->srcPath, "r");
    do {
        line = fgets(line, 1024*100, fp);
        if (line != NULL) {
            len = strlen(line);
            destLine = malloc(len+1);
            strncpy(destLine, line, len);
            destLine[len] = '\0';
            printf("line ==> %s\n", line);
            printf("destline ==> %s\n", destLine);
            
            // 解析文件，如果没有命令就直接写到临时文件
            point = searchCmdIndex(destLine);
            if (point == NULL) {
                // 原始字符串
                appendDestLine(dest->startList, destLine);
                continue;
            }
            cmd = parseCmdStr(extraCmdStr(destLine, point));
            if (cmd == NULL) {
                // 原始字符串
                appendDestLine(dest->startList, destLine);
                continue;
            }
            //TODO pre为空的时候需要判断
            char *pre = htSubstr(src, cmd->point->start);
            appendDestLine(dest->startList, pre);
            appendDestLine(dest->endList, src + cmd->point->end + 1);

            cmd->point = point;
            // 被识别的类型，需要解析
            if (cmd->file_type == FILE_TYPE_HTML) {
               parseHtml(destList, destLine, cmd);
            }
        }
    } while(line != NULL);
}

void buildRootFile(char *rootFile) {
    filedest *dest = malloc(sizeof(filedest));
    dest->srcPath = rootFile;
    dest->cmd = NULL;
    dest->startList = htCreateList();
    dest->middle = htCreateList();
    dest->endList = htCreateList();
    buildFile(dest);
}


