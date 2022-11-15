
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

// replace part str  
char * replacePart(char *src, int start, int end, char *part);
char * replacePartUseIndex(char *src, charindex, char *part);
char * htSubstr(char *src, int n);

// file fuction
char * getAbsolutePath(char *path);
int isDir(char *file);
int isFile(char *file);
int isExist(char *file);
void createDir(char *dirPath);
FILE *deleteAndCreateFile(char *filePath);

