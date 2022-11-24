#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"

/**
 * 从命令中解析出包含的文件列表
 */
void parseCmdStr(cmdentity *entity) {
    int cmdType = entity->cmd_type;
    void *strret = entity->strret;
    char *cmd_src = entity->src_cmd;
    int cmdLen = strlen(cmd_src);
    printf("cmd_src:%s\n", cmd_src);

    // 解析|分割字符
    int start = 7, nextstart = 7;
    printf("==============>cmdType:%d, cmdtypepath:%d\n", cmdType, CMD_TYPE_PATH);
    if (cmdType & CMD_TYPE_PATH) {
        printf("path\n");
        entity->strret = htSubstr(cmd_src+nextstart, cmdLen-1-nextstart);
        printf("找path:%s\n", entity->strret);
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
    printf("list:len:%d, srcItemList:%d\n",((htlist *)strret)->len, srcItemList->len);
    htnode *tmpNode = srcItemList->head;
    while(tmpNode != NULL) {
        printf("node data:%s\n", tmpNode->data);
        abPath = getAbsolutePath(tmpNode->data);
        ret = strchr(tmpNode->data, '*');
        if (ret == NULL) {
            if (isFile(abPath)) {
                htAddNodeUseData(strret, abPath);
            } else if(isDir(abPath)) {
                printf("=========> start abPath:%s\n", abPath); 
                htfilerecursive(strret, abPath);
                printf("=========> end abPath:%s\n", abPath); 
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
    printf("尝试解析命令:%s\n", cmdStr);
    if (strncmp("html", cmdStr + 2, (size_t)4) == 0) {
        entity->cmd_type = CMD_TYPE_HTML;
    } else if (strncmp("mark", cmdStr + 2, (size_t)4) == 0) {
        entity->cmd_type = CMD_TYPE_MARK;
    } else if (strncmp("path", cmdStr + 2, (size_t)4) == 0) {
        entity->cmd_type = CMD_TYPE_PATH;
    } else {
        printf("未知的命令类型:%s\n", cmdStr);
    }
    parseCmdStr(entity);
    printf("===>解析命令完成\n");
    if ( entity->cmd_type & CMD_TYPE_HTML 
            || entity->cmd_type & CMD_TYPE_MARK) {
        printf("===>文件夹列表\n");
        htlist *filelist = (htlist *)(entity->strret);
        printf("===>文件夹列表强制类型转换成, 文件数量:%d\n", filelist->len);
        if (filelist->len > 1) {
            printf("===>cmd type mutl!\n");
            entity->cmd_type = entity->cmd_type | CMD_TYPE_MUTL;
        }
    }
    if ( entity->cmd_type & CMD_TYPE_PATH) {
        printf("================>path:%s\n", entity->strret);
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
    int pre = -1, incmd = 0;
    char tmp;
    printf("输入行:%s, 长度:%d\n", srcLine, len);
    for (int i=0; i<len; i++) {
        tmp = srcLine[i];
        if (pre>=0 && incmd) {
            if (tmp == '}') {
                printf("找到命令中\n");
                point->start = pre;
                point->end = i;
                return point;
            }
        } else if(pre>=0 && !incmd) {
            if (tmp == '{') {
                printf("命令中\n");
                incmd = 1;
            } else {
                pre = -1;
            }
        } else {
            if (tmp == '@') {
                printf("找到pre\n");
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
            printf("文件夹结束:%s\n", dest->cur_file);
            break;
        }
        printf("开始解析行:%s\n", line);
        destLine = htStrCpy(line);
        printf("原始数据复制:%s\n", destLine);
            
        // 解析文件，如果没有命令就直接写到临时文件
        point = searchCmdIndex(destLine);
        if (point == NULL) {
            // 原始字符串
            printf("无命令行, 原始数:%s\n",destLine);
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
        
        if ((cmd->cmd_type & CMD_TYPE_PATH) || (cmd->cmd_type & CMD_TYPE_MUTL)) {
            printf("==============> $$$$$$$$$$cmd type!\n");
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

void extraPath(htlist *pathList, buildcontext *dest) {
    printf("extraPath:%s\n", dest->cur_file);
    htCleanList(pathList);
    printf("extraPath:clean path liest\n");

    htnode *tmp = dest->retList->head;
    do {
        linedest *line = (linedest *)tmp->data;
        if (line->line_type == LINE_TYPE_CMD) {
            cmdentity *entity = (cmdentity *)line->data;
            if (entity->cmd_type & CMD_TYPE_PATH) {
                printf("extraPath:src cmd:%s\n", entity->src_cmd);
                htAddNodeUseData(pathList, entity->strret);
            }
        }        
        tmp = tmp->nextNode;
    } while(tmp != NULL);
    printf("extraPath:stop\n");
}

cmdentity * extramutl(buildcontext *dest) {
    cmdentity *mutlentity = NULL;
    htnode *tmp = dest->retList->head;
    do {
        linedest *line = (linedest *)tmp->data;
        if (line->line_type == LINE_TYPE_CMD) {
            cmdentity *entity = (cmdentity *)line->data;
            if (entity->cmd_type & CMD_TYPE_MUTL) {
                mutlentity = (cmdentity *)line->data;
            }
        }        
        tmp = tmp->nextNode;
    } while(tmp != NULL);
    return mutlentity;
}

void writeLine(FILE *fp, buildcontext *dest) {
    htnode *tmp = dest->retList->head;
    do {
        linedest *line = (linedest *)tmp->data;
        if ( line->line_type == LINE_TYPE_HTML) {
            // 写数据
            fputs(line->data ,fp);
            printf("html 类型行数据:%s\n", line->data);
        } else if (line->line_type == LINE_TYPE_CMD) {
            cmdentity *entity = (cmdentity *)line->data;
            if (!(entity->cmd_type & CMD_TYPE_PATH)) {
                fputs(entity->src_cmd ,fp);
            }
            printf("cmd 类型行数据, 原始命令:%s\n", entity->src_cmd);
        } else {
            printf("未知的行数据类型\n");
        }
        tmp = tmp->nextNode;
    } while(tmp != NULL);
}

void writeMulLine(FILE *fp, buildcontext *dest, buildcontext *mulItemDest) {
    printf("write mul file!\n");
    htnode *tmp = dest->retList->head;
    do {
        printf("write while!\n");
        linedest *line = (linedest *)tmp->data;
        if ( line->line_type == LINE_TYPE_HTML) {
             // 写数据
             fputs(line->data ,fp);
             printf("html 类型行数据:%s\n", line->data);
        } else if (line->line_type == LINE_TYPE_CMD) {
             cmdentity *entity = (cmdentity *)line->data;
             if (entity->cmd_type & CMD_TYPE_MUTL) {
                 writeLine(fp, mulItemDest);
             }
             printf("cmd 类型行数据, 原始命令:%s\n", entity->src_cmd);
        } else {
             printf("未知的行数据类型\n");
        }
        tmp = tmp->nextNode;
    } while(tmp != NULL);
}

/**
 * 生成目标文件
 */
char * destFile(htlist *prePathList, htlist *tmpPathList) {
    int len = prePathList->len + tmpPathList->len;
    char * strList[len];
    int i = -1;
    htnode *tmp = prePathList->head;
    while(tmp != NULL)  {
        i++;
        strList[i] = tmp->data;
        tmp = tmp->nextNode;
    }
    tmp = tmpPathList->head;
    while(tmp != NULL)  {
        i++;
        strList[i] = tmp->data;
        tmp = tmp->nextNode;
    }
    return htContact(strList, i+1);
}

void buildRootFile(char *rootFile) {
    buildcontext *dest = malloc(sizeof(buildcontext));
    dest->src_file = rootFile;
    dest->cur_file = rootFile;
    dest->dest_file= NULL;
    dest->retList= htCreateList();
    buildFile(dest);

    htlist *tmpPathList = htCreateList();
    htlist *prePathList = htCreateList();

    extraPath(prePathList, dest);
    cmdentity *mutlentity = extramutl(dest);

    if (mutlentity != NULL) {
        htlist *fileList = (htlist *)mutlentity->strret;
        htnode *tmpfilenode = fileList->head;
        while(tmpfilenode != NULL) {
            // 编译每个文件
            printf("\n\n\nbuild partfile start: %s\n", tmpfilenode->data);
            buildcontext *muItemDest = malloc(sizeof(buildcontext));
            muItemDest->src_file = tmpfilenode->data;
            muItemDest->cur_file = tmpfilenode->data;
            muItemDest->dest_file= NULL;
            muItemDest->retList= htCreateList();
            buildFile(muItemDest);

            extraPath(tmpPathList, muItemDest);

            printf("start create destFilePath\n");

            dest->dest_file = destFile(prePathList, tmpPathList);
            printf("end create destFilePath, dest_file:%s\n", dest->dest_file);

            FILE *fp = deleteAndCreateFile(dest->dest_file);
            printf("finish create destFile\n");
            writeMulLine(fp, dest, muItemDest);
            printf("finish write hestFile\n");
            fclose(fp);
            printf("\n\n\nbuild partfile end: %s\n", tmpfilenode->data);
            tmpfilenode = tmpfilenode->nextNode;
        }
    } else {
        dest->dest_file = destFile(prePathList, tmpPathList);
        FILE *fp = deleteAndCreateFile(dest->dest_file);
        writeLine(fp, dest);
        fclose(fp);
    }
}


