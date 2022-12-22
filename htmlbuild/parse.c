#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"
#include "config.h"

/**
 * 生成目标文件
 */
char * destFile(htlist *prePathList, htlist *tmpPathList) {
    int len = prePathList->len + tmpPathList->len;
    if (len <= 0) {
        log_error("not set path!");
        exit(1);
    }
    char * strList[len+1];
    strList[0] = htconfig.dest_dir;
    int i = 0;
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

char *build_file_path(char *file) {
    log_info("file:%s", file);
    if (file[0] == '/') {
        return file;
    }
    log_info("src_dir:%s, file:%s", htconfig.src_dir, file);
    return htContactTwoStr(htconfig.src_dir, file);
}

void parse_init_filepool() {
    if (filepool.inited) {
        return;
    }
    filepool.childfiledict = htDictCreate(1024);
    filepool.rootfiledict = htDictCreate(1024);
    filepool.rootfilelist = htCreateList();
    filepool.inited = 1;
}

void registRootFile(char *rootFile) {
    htlist *childlist = htDictGet(filepool.rootfiledict, rootFile);
    if (childlist == NULL) {
        htAddNodeUseData(filepool.rootfilelist, rootFile);
    }
}

void registFile(char *file, char *rootFile) {
    // 注册子文件字典
    htlist *rootlist = htDictGet(filepool.childfiledict, file);
    if (rootlist == NULL) {
        rootlist = htCreateList();
        htDictPutNoFreeValue(filepool.childfiledict, file, rootlist);
    }
    htnode *rootFileNode = NULL;
    htnode *tmp = rootlist->head;
    while (tmp != NULL) {
        if (strlen(tmp->data) != strlen(rootFile)) {
            tmp = tmp->nextNode;
            continue;
        }
        if(strcmp(rootFile, tmp->data) == 0) {
            rootFileNode = tmp;
            break;
        }
        tmp = tmp->nextNode;
    }
    if (rootFileNode == NULL) {
        htAddNodeUseData(rootlist, rootFile);
    }

    htlist *childlist = htDictGet(filepool.rootfiledict, rootFile);
    if (childlist == NULL) {
        childlist = htCreateList();
        htDictPutNoFreeValue(filepool.rootfiledict, rootFile, childlist);
    }
    htnode *childFileNode = NULL;
    tmp = childlist->head;
    while (tmp != NULL) {
        if (strlen(tmp->data) != strlen(file)) {
            tmp = tmp->nextNode;
            continue;
        }
        if(strcmp(file, tmp->data) == 0) {
            childFileNode = tmp;
            break;
        }
        tmp = tmp->nextNode;
    }
    if (childFileNode == NULL) {
        htAddNodeUseData(childlist, file);
    }
}

void cleanFromRootFile(char *rootFile) {
    htlist *childlist = htDictGet(filepool.rootfiledict, rootFile);
    if (childlist == NULL) {
        return;
    }

    htlist *rootlist = NULL; 
    htnode *tmp = childlist->head;
    htnode *roottmp = NULL;
    while(tmp != NULL) {
        // 子节点的root列表中删除该rootfile
        char *childfile = tmp->data;
        rootlist = htDictGet(filepool.childfiledict, childfile);
        if (rootlist == NULL) {
            tmp=tmp->nextNode;
            continue;
        }
        roottmp = rootlist->head;
        while(roottmp != NULL) {
            if (strlen(roottmp->data) != strlen(rootFile)) {
                tmp = tmp->nextNode;
                continue;
            }
            if(strcmp(rootFile, roottmp->data) == 0) {
                break;
            }
            roottmp = roottmp->nextNode;
        }
        if (roottmp != NULL) {
            htRemoveNode(rootlist, roottmp);
        }

        tmp=tmp->nextNode;
    }
    htDictRemoveNoFreeValue(filepool.rootfiledict, rootFile);
    roottmp = filepool.rootfilelist->head;
    while(roottmp != NULL) {
        if (strlen(roottmp->data) != strlen(rootFile)) {
            tmp = tmp->nextNode;
            continue;
        }
        if(strcmp(rootFile, roottmp->data) == 0) {
            break;
        }
        roottmp = roottmp->nextNode;
    }
    if (roottmp != NULL) {
        htRemoveNode(filepool.rootfilelist, roottmp);
    }
}

htlist *parseGetRootFile(char *fileName) {
    return htDictGet(filepool.childfiledict, fileName);
}

/**
 * 从命令中解析出包含的文件列表
 */
void parseCmdStr(cmdentity *entity) {
    int cmdType = entity->cmd_type;
    void *strret = entity->strret;
    char *cmd_src = entity->src_cmd;
    int cmdLen = strlen(cmd_src);

    // 解析|分割字符
    log_info("cmd str:%s", cmd_src);
    int start = 7, nextstart = 7;
    if (cmdType & CMD_TYPE_PATH) {
        entity->strret = htSubstr(cmd_src+nextstart, cmdLen-1-nextstart);
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
    htnode *tmpNode = srcItemList->head;
    while(tmpNode != NULL) {
        log_info("tmpdata:%s",tmpNode->data);
        abPath = build_file_path(tmpNode->data);
        log_info("abpath:%s",abPath);
        ret = strchr(tmpNode->data, '*');
        if (ret == NULL) {
            if (isFile(abPath)) {
                htAddNodeUseData(strret, abPath);
            } else if(isDir(abPath)) {
                htfilerecursive(strret, abPath);
            } else {
                log_info("解析命令:%s, 路径:%s, 不是文件也不是文件夹", cmd_src, abPath );
            }
        } else {
            // TODO 完善模糊匹配

        }
        tmpNode = tmpNode->nextNode;
    }
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
    log_info("==>start parseCmd cmdStr:%s", cmdStr);
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
        log_info("未知的命令类型:%s\n", cmdStr);
    }
    parseCmdStr(entity);
    if ( entity->cmd_type & CMD_TYPE_HTML 
            || entity->cmd_type & CMD_TYPE_MARK) {
        htlist *filelist = (htlist *)(entity->strret);
        if (filelist->len > 1) {
            entity->cmd_type = entity->cmd_type | CMD_TYPE_MUTL;
        }
    }
    if ( entity->cmd_type & CMD_TYPE_PATH) {
        log_info("==>parseCmd path:%s", entity->strret);
    }
    log_info("==>end parseCmd cmdStr:%s", cmdStr);
    return entity;
}


char * extraCmdStr(char *cmdStr, charindex *point) {
    return htSubstr(cmdStr+point->start, point->end - point->start + 1);
}

void appendDestLine(htlist *destList, char * destLine) {
    if (destLine == NULL) {
        return;
    }
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
    for (int i=0; i<len; i++) {
        tmp = srcLine[i];
        if (pre>=0 && incmd) {
            if (tmp == '}') {
                point->start = pre;
                point->end = i;
                return point;
            }
        } else if(pre>=0 && !incmd) {
            if (tmp == '{') {
                incmd = 1;
            } else {
                pre = -1;
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
    dest->cur_file = build_file_path(dest->cur_file);
    log_info("=>buidFile file:%s", dest->cur_file);
    if (!isFile(dest->cur_file)) {
        log_error("=>buidFile error not find file:%s", dest->cur_file);
        return;
    }

    registFile(dest->cur_file, dest->src_file);

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
        
        if ((cmd->cmd_type & CMD_TYPE_PATH) || (cmd->cmd_type & CMD_TYPE_MUTL)) {
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
    log_info("end build file:%s", dest->cur_file);
}

void extraPath(htlist *pathList, buildcontext *dest) {
    htCleanList(pathList);

    htnode *tmp = dest->retList->head;
    do {
        linedest *line = (linedest *)tmp->data;
        if (line->line_type == LINE_TYPE_CMD) {
            cmdentity *entity = (cmdentity *)line->data;
            if (entity->cmd_type & CMD_TYPE_PATH) {
                htAddNodeUseData(pathList, entity->strret);
            }
        }        
        tmp = tmp->nextNode;
    } while(tmp != NULL);
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

void htwrite(char *str, FILE *fp) {
    int len = strlen(str);
    for (int i=0;i<len;i++) {
        fputc(str[i], fp);
    }
}

void writeLine(FILE *fp, buildcontext *dest) {
    htnode *tmp = dest->retList->head;
    do {
        linedest *line = (linedest *)tmp->data;
        if ( line->line_type == LINE_TYPE_HTML) {
            // 写数据
            htwrite(line->data ,fp);
        } else if (line->line_type == LINE_TYPE_CMD) {
            cmdentity *entity = (cmdentity *)line->data;
            if (!(entity->cmd_type & CMD_TYPE_PATH)) {
                htwrite(entity->src_cmd ,fp);
            }
        } else {
            log_info("未知的行数据类型\n");
        }
        tmp = tmp->nextNode;
    } while(tmp != NULL);
}

void writeMulLine(FILE *fp, buildcontext *dest, buildcontext *mulItemDest) {
    htnode *tmp = dest->retList->head;
    do {
        linedest *line = (linedest *)tmp->data;
        if ( line->line_type == LINE_TYPE_HTML) {
            // 写数据
            htwrite(line->data ,fp);
        } else if (line->line_type == LINE_TYPE_CMD) {
             cmdentity *entity = (cmdentity *)line->data;
             if (entity->cmd_type & CMD_TYPE_MUTL) {
                writeLine(fp, mulItemDest);
             }
        } else {
             printf("未知的行数据类型\n");
        }
        tmp = tmp->nextNode;
    } while(tmp != NULL);
}



void buildRootFile(char *rootFile) {
    // todo skip file
    log_info("buildRootFile=>build root file:%s", rootFile);
    parse_init_filepool();
    cleanFromRootFile(rootFile);

    buildcontext *dest = malloc(sizeof(buildcontext));
    dest->src_file = rootFile;
    dest->cur_file = rootFile;
    dest->dest_file= NULL;
    dest->retList= htCreateList();
    registRootFile(rootFile);
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
            buildcontext *muItemDest = malloc(sizeof(buildcontext));
            muItemDest->src_file = rootFile;
            muItemDest->cur_file = build_file_path(tmpfilenode->data);
            muItemDest->dest_file= NULL;
            muItemDest->retList= htCreateList();
            log_info("buildRootFile=>build child file:%s", muItemDest->cur_file);
            buildFile(muItemDest);

            extraPath(tmpPathList, muItemDest);

            dest->dest_file = destFile(prePathList, tmpPathList);
            log_info("buildRootFile=>destFile:%s", dest->dest_file);

            FILE *fp = deleteAndCreateFile(dest->dest_file);
            writeMulLine(fp, dest, muItemDest);
            fclose(fp);
            log_info("buildRootFile=>end build child file:%s", muItemDest->cur_file);
            tmpfilenode = tmpfilenode->nextNode;
        }
    } else {
        dest->dest_file = destFile(prePathList, tmpPathList);
        log_info("buildRootFile=>destFile:%s", dest->dest_file);
        FILE *fp = deleteAndCreateFile(dest->dest_file);
        writeLine(fp, dest);
        fclose(fp);
    }
    log_info("end build root file:%s", rootFile);
}

void buildChildFile(char *childFile) {
    log_info("build root file begin child file:%s!\n", childFile);
    htlist *rootlist = htDictGet(filepool.childfiledict, childFile);
    if (rootlist == NULL) {
        return;
    }
    htnode *tmpNode = rootlist->head;
    while (tmpNode != NULL) {
        buildRootFile(tmpNode->data);
        tmpNode = tmpNode->nextNode;
    }
    log_info("end build root file begin child file:%s!\n", childFile);
}
