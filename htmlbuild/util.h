#include "list.h"

#ifndef HT_UTIL
#define HT_UTIL

#define IS_FILE 1
#define IS_DIR 2

typedef struct ht_file {
    int type;
    char *filePath;
} htfile;

typedef struct htfile_list {
    htfile *head;
    htfile *end;
} htfilelist;

typedef struct char_index {
    int start;
    int end;
} charindex;

#endif

// replace part str  
char * replacePart(char *src, int start, int end, char *part);
char * replacePartUseIndex(char *src, charindex, char *part);
char * htSubstr(char *src, int n);
char * htStrCpy(char *src);
char * htContactTwoStr(char *str1, char *str2);
char * htContact(char* strList[], int length);

// file fuction
char *currentDir();
char * getAbsolutePath(char *path);
int isDir(char *file);
int isFile(char *file);
int isExist(char *file);
int isDirChild(char *dirPath, char *childPath);
int htDirIsParentDir(char *parentDir, char *childPath);
long fileUpdateTime(char *file);
void createDir(char *dirPath);
FILE *deleteAndCreateFile(char *filePath);
htlist * htfilerecursive(htlist *filelist, char *basePath);
htlist * htfilerecursivedetail(htlist *filelist, char *basePath, int onlyfile);
htlist * htdirchildren(htlist *filelist, char *basePath);

void log_info(const char *fmt, ...);
void log_error(const char *fmt, ...);
