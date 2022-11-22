#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"

/**
 * 从命令中解析出包含的文件列表
 */
void parseCmdStr(int cmdType, void *strret, char *cmd_src) {
    int cmdLen = strlen(cmd_src);
    printf("cmd_src:%s\n", cmd_src);

    // 解析|分割字符
    int start = 7, nextstart = 7;
    if (cmdType & CMD_TYPE_PATH) {
        strret = htSubstr(cmd_src+nextstart, cmdLen-1-nextstart);
        return;
    }

    htlist *srcItemList = htCreateList();
    for (int i=start; i<cmdLen-1; i++) {
        if (cmd_src[i] != '|') {
            continue;
        }
        htAddNodeUseData(srcItemList, htSubstr(cmd_src+nextstart, i-nextstart));
        nextstart = i + 1;
    }
    if (nextstart < cmdLen-1) {
        htAddNodeUseData(srcItemList, htSubstr(cmd_src+nextstart, cmdLen-1-nextstart));
    }

    char * abPath = NULL;
    char * ret = NULL;
    printf("list:len:%d\n",((htlist *)strret)->len);
    htnode *tmpNode = srcItemList->head;
    while(tmpNode != NULL) {
        printf("node data:%s\n", tmpNode->data);
        abPath = getAbsolutePath(tmpNode->data);
        ret = strchr(tmpNode->data, '*');
        if (ret == NULL) {
            if (isFile(abPath)) {
                htAddNodeUseData(strret, abPath);
            } else if(isDir(abPath)) {
                htfilerecursive(strret, abPath);
            } else {
                printf("解析命令:%s, 路径:%s, 不是文件也不是文件夹\n", cmd_src, abPath );
            }
        } else {
            // TODO 完善模糊匹配

        }
        tmpNode = tmpNode->nextNode;
    }
    printf("list:len:%d\n",((htlist *)strret)->len);
}

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
cmdentity * parseCmd(char *cmdStr) {
    int len = strlen(cmdStr);
    char tmp;
    int charIndex = 0;
    cmdentity *entity = malloc(sizeof(cmdentity)); 
    entity->src_cmd = cmdStr;
    entity->cmd_type = CMD_TYPE_NONE;
    entity->strret = htCreateList();
    if (strncmp("html", cmdStr + 2, (size_t)4) == 0) {
        entity->cmd_type = CMD_TYPE_HTML;
    } else if (strncmp("mark", cmdStr + 2, (size_t)4) == 0) {
        entity->cmd_type = CMD_TYPE_MARK;
    } else if (strncmp("path", cmdStr + 2, (size_t)4) == 0) {
        entity->cmd_type = CMD_TYPE_PATH;
    } else {
        printf("未知的命令类型:%s\n", cmdStr);
    }
    parseCmdStr(entity->cmd_type, entity->strret, cmdStr);
    printf("===>解析命令完成\n");
    if ( entity->cmd_type & CMD_TYPE_HTML 
            || entity->cmd_type & CMD_TYPE_MARK) {
        printf("===>文件夹列表\n");
        htlist *filelist = (htlist *)(entity->strret);
        printf("===>文件夹列表强制类型转换成\n");
        printf("===>文件夹列表强制类型转换成, 文件数量:%d\n", filelist->len);
        if (filelist->len > 1) {
            entity->cmd_type = entity->cmd_type & CMD_TYPE_MUTL;
        }
        printf("===>文件夹列表2\n");
    }
    return entity;
}


char * extraCmdStr(char *cmdStr, charindex *point) {
    return htSubstr(cmdStr+point->start, point->end - point->start + 1);
}

void appendDestLine(htlist *destList, char * destLine) {
    linedest *line = malloc(sizeof(linedest));
    line->line_type = LINE_TYPE_HTML;
    line->data = destLine;
    htAddNodeUseData(destList, line);
}

void appendDestCmd(htlist *destList, cmdentity * cmd) {
    linedest *line = malloc(sizeof(linedest));
    line->line_type = LINE_TYPE_CMD;
    line->data = cmd;
    htAddNodeUseData(destList, line);
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

void buildFile(buildcontext *dest) {
    char *line, *destLine;
    charindex *point;
    cmdentity *cmd;
    htlist *middleFileList;
    char *oldCurFile = dest->cur_file;
    int len;
    line = malloc(1024*100);
    FILE *fp = fopen(dest->cur_file, "r");
    do {
        line = fgets(line, 1024*100, fp);
        if (line == NULL) {
            break;
        }
        destLine = htStrCpy(line);
            
        // 解析文件，如果没有命令就直接写到临时文件
        point = searchCmdIndex(destLine);
        if (point == NULL) {
            // 原始字符串
            appendDestLine(dest->retList, destLine);
            continue;
        }
        cmd = parseCmd(extraCmdStr(destLine, point));
        if (cmd == NULL) {
            // 原始字符串
            appendDestLine(dest->retList, destLine);
            continue;
        }

        char *pre = htSubstr(destLine, point->start);
        appendDestLine(dest->retList, pre);
        
        if (cmd->cmd_type & CMD_TYPE_PATH || cmd->cmd_type & CMD_TYPE_MUTL) {
            appendDestCmd(dest->retList, cmd);
            appendDestLine(dest->retList, destLine + point->end + 1);
            continue;
        }
        oldCurFile = dest->cur_file;
        htlist *filelist = (htlist *)cmd->strret;
        dest->cur_file = filelist->head->data;
        buildFile(dest);
        dest->cur_file = oldCurFile;
        appendDestLine(dest->retList, destLine + point->end + 1);
    } while(line != NULL);
}

void buildRootFile(char *rootFile) {
    buildcontext *dest = malloc(sizeof(buildcontext));
    dest->src_file = rootFile;
    dest->cur_file = rootFile;
    dest->dest_file= NULL;
    dest->retList= htCreateList();
    buildFile(dest);
    htnode *tmp = dest->retList->head;
    do {
        linedest *line = (linedest *)tmp->data;
        if ( line->line_type == LINE_TYPE_HTML) {
            printf("html 类型行数据:%s\n", line->data);
        } else if (line->line_type == LINE_TYPE_CMD) {
            cmdentity *entity = (cmdentity *)line->data;
            printf("cmd 类型行数据, 原始命令:%s\n", entity->src_cmd);
        } else {
            printf("未知的行数据类型\n");
        }
        tmp = tmp->nextNode;
    } while(tmp != NULL);
    // 遍历节点依次写入获取dest_file
    // 循环多文件命令, 遍历节点写入文件
}


