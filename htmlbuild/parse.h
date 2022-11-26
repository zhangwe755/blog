
#include "util.h"
#include "list.h"
#include "dict.h"

#ifndef PARSE_FILE
#define PARSE_FILE

#define LINE_TYPE_HTML 0
#define LINE_TYPE_CMD 1
#define LINE_TYPE_CMD_RET 2

#define CMD_TYPE_NONE 1 << 1
#define CMD_TYPE_HTML 1 << 2
#define CMD_TYPE_MARK 1 << 3
#define CMD_TYPE_PATH 1 << 4
#define CMD_TYPE_MUTL 1 << 8

typedef struct cmd_entity {
    int cmd_type;
    char *src_cmd;
    void *strret;
} cmdentity;

typedef struct line_dest {
    int line_type;
    void *data;
} linedest;

typedef struct root_cmd_list {
    char *file_content;
    htlist *cmdentitylist;
} rootcmdlist;

typedef struct file_dest {
    char *filePath;
    htlist *contentList;
} filedest;

typedef struct build_context {
    char *src_file;
    char *cur_file;
    char *dest_file;
    htlist *retList;
} buildcontext;

typedef struct parse_file_pool {
    int inited;
    /**
     * childfile : rootfilelist
     */
    htdict *childfiledict;
    /**
     * rootfile: childfilelist;
     */
    htdict *rootfiledict;

    /**
     * 所有rootfile，list
     */
    htlist *rootfilelist;

} parsefilepool;

parsefilepool filepool;

#endif

htlist *parseGetRootFile(char *fileName);


// 结构体不能返回空
charindex * searchCmdIndex(char *srcLine);

void parseHtml(htlist *destlist, char *src, cmdentity *cmd);

cmdentity * parseCmd(char *cmdStr);

void appendDestLine(htlist *destList, char * destLine);

void buildRootFile(char *rootFile);
