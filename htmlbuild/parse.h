
#include "util.h"
#include "list.h"

#ifndef PARSE_FILE
#define PARSE_FILE

#define LINE_TYPE_HTML 0
#define LINE_TYPE_CMD 1
#define LINE_TYPE_PATH 2

typedef struct cmd_entity {
    int file_type;
    char *src_cmd;
    htlist *file_list;
} cmdentity;

typedef struct line_dest {
    int line_type;
    void *data;
} linedest;

typedef struct line_cmd {
    cmdentity *cmd;
    htlist *lines;
} linecmd;

typedef struct root_cmd_list {
    char *file_content;
    htlist *cmdentitylist;
} rootcmdlist;

typedef struct file_dest {
    char *srcPath;
    cmdentity *cmd;
    htlist *startList;
    htlist *middle;
    htlist *endList;
} filedest;

typedef struct build_context {
    char *src_file;
    char *dest_file;
    filedest * dest;
} buildcontext;

#endif

void parseCmdFiles(htlist *files, char *cmd_src);

// 结构体不能返回空
charindex * searchCmdIndex(char *srcLine);

void parseHtml(htlist *destlist, char *src, cmdentity *cmd);

cmdentity * parseCmdStr(char *cmdStr);

void appendDestLine(htlist *destList, char * destLine);

//void parseFile(htlist *destList, char *rootFile);

void buildFile(char *rootFile);
