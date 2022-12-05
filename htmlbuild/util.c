#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/stat.h>
#include <dirent.h>


#include "util.h"
#include "list.h"

// start end 下标都会被替换
char * replacePart(char *src, int start, int end, char *part) {
    char *dest = malloc(strlen(src) + strlen(part) + end - start);
    if (start > 0) {
        strncat(dest, src, start);
    }
    strcat(dest, part);
    if (end < strlen(src)-1) {
        strcat(dest, src + end + 1);
    }
    return dest;
}


char * replacePartUseIndex(char *src, charindex point, char *part) {
    return replacePart(src, point.start, point.end, part);
}

char * htSubstr(char *src, int n) {
    char *dest = malloc(n+1);
    strncpy(dest, src, n);
    dest[n] = '\0';
    return dest;
}

char * htStrCpy(char *src) {
    int len = strlen(src);
    char *dest = malloc(len+1);
    strncpy(dest, src, len);
    dest[len] = '\0';
    return dest;
}

char * htContactTwoStr(char *str1, char *str2) {
    char *strList[2];
    strList[0] = str1;
    strList[1] = str2;
    return htContact(strList, 2);
}

char * htContact(char* strList[], int length) {
    int len = 0;
    for (int i=0;i<length;i++) {
        len += strlen(strList[i]);
    }
    if (len == 0) {
        return NULL;
    }
    char *dest = malloc(len+1);
    for (int i=0;i<length;i++) {
        strcat(dest, strList[i]);
    }
    dest[len] = '\0';
    return dest;
}



///////////// file function 

char *currentDir() {
    char buf[1024];
    getcwd(buf, sizeof(buf));
    return htStrCpy(buf);
}

char* getAbsolutePath(char *path) {
    char buf[1024];
    char *abPath;
    size_t pathStart = 0;
    printf("===>path:%s\n", path);
    if (path[0] == '/') {
        abPath = malloc(strlen(path)+1);
        strcpy(abPath, path);
        abPath[strlen(path)] = '\0';
        printf("==>abPath : %s abPathSize:%lu, strlen:%lu\n", abPath, sizeof(abPath), strlen(abPath));
        return abPath;
    }
    if (strlen(path)>2 && path[0] == '.' && path[1] == '/') {
        pathStart = 2;
    }
    getcwd(buf, sizeof(buf));
    abPath = malloc(strlen(buf) + strlen(path) + 1 - pathStart);
    strcpy(abPath, buf);
    strcat(abPath, "/");
    strcat(abPath, path + pathStart);
    printf("abPath : %s abPathSize:%lu, strlen:%lu\n", abPath, sizeof(abPath), strlen(abPath));
    return abPath;
}

/**
 * child是否是all开头的字母
 */
int htDirIsParentDir(char *parentDir, char *childPath) {
    int parentlen = strlen(parentDir);
    int childlen = strlen(childPath);
    if (childlen < parentlen) {
        return 0;
    }
    if(childlen>parentlen && childPath[parentlen]!='/') {
        return 0;
    }
    if(strncmp(parentDir, childPath, parentlen)==0) {
        return 1;
    }
    return 0;
}

int isDirChild(char *dirPath, char *childPath) {
    int has_end = 0;
    char *tmpdir = dirPath;
    if (dirPath[strlen(dirPath)-1] == '/') {
        tmpdir = htSubstr(dirPath, strlen(dirPath)-1);
        has_end = 1;
    }
    char *tmpchild = htStrCpy(childPath);
    tmpchild = dirname(tmpchild);
    int result = strcmp(tmpdir, tmpchild) == 0 ? 1 : 0;
    free(tmpchild);
    if (has_end) {
        free(tmpdir);
    }
    return result;
}

int isDir(char *file) {
    if (file == NULL) {
        return 0;
    }
    struct stat buf;
    int result = stat(file, &buf);
    if (result != 0) {
        return 0;
    }
    // 需不需要释放内存
    return S_IFDIR & buf.st_mode;
}

int isFile(char *file) {
    if (file == NULL) {
        return 0;
    }
    struct stat buf;
    int result = stat(file, &buf);
    if (result != 0) {
        return 0;
    }
    // 需不需要释放内存
    return S_IFREG & buf.st_mode;
}

int isExist(char *file) {
    struct stat buf;
    int result = stat(file, &buf);
    if (result != 0) {
        return 0;
    }
    // 需不需要释放内存
    printf("result : %d, mode: %d, %d, %d\n", result, buf.st_mode, S_IFDIR, S_IFREG);
    return (S_IFDIR & buf.st_mode) || (S_IFREG & buf.st_mode) ? 1 : 0;
}

long fileUpdateTime(char *file) {
    struct stat buf;
    int result = stat(file, &buf);
    if (result != 0) {
        return 0;
    }
    return buf.st_mtime;
}

void createDir(char *dirPath) {
    if (isExist(dirPath)){
        return;
    }
    char *path[1000]; 
    int i = 0;
    char *tmp = dirPath;
    while (1) {
        char *old = malloc(strlen(tmp));;
        strcpy(old, tmp);
        path[i] = old;
        tmp = dirname(tmp);
        if (isExist(tmp)) {
            break;
        }
        i++;
    }
    for (;i>=0;i--) {
        mkdir(path[i], 0777);
        free(path[i]);
    }
}

FILE *deleteAndCreateFile(char *filePath) {
    printf("function deleteAndCreateFile:%s\n", filePath);
    if (isFile(filePath)) {
        remove(filePath);
    }
    printf("function deleteAndCreateFile clear file:%s\n", filePath);
    char *src = malloc(strlen(filePath));
    printf("function deleteAndCreateFile src file===>:%s\n", filePath);
    strcpy(src, filePath);
    printf("function deleteAndCreateFile src file:%s\n", src);
    filePath = dirname(filePath);
    printf("function deleteAndCreateFile filePath:%s\n", filePath);
    createDir(filePath);
    FILE *fp = fopen(src, "w");
    return fp;
}

htlist * htfilerecursivedetail(htlist *filelist, char *basePath, int onlyfile) {
    // 路径
    char path[10240];
    struct dirent *dp;
    char *tmpPath;
    DIR *dir;
    int len;

   	// 如果是文件则直接结束此次调用
    if (isFile(basePath)) {
        len = strlen(basePath);
        tmpPath = malloc(len+1);
        strcpy(tmpPath, basePath);
        tmpPath[len] = '\0';
        printf("环获取文件:%s\n", tmpPath);
        htAddNodeUseData(filelist, tmpPath);
        return filelist;
    }
    if (!onlyfile) {
        len = strlen(basePath);
        tmpPath = malloc(len+1);
        strcpy(tmpPath, basePath);
        tmpPath[len] = '\0';
        printf("环获取文件夹:%s\n", tmpPath);
        htAddNodeUseData(filelist, tmpPath);
    }
    dir = opendir(basePath);
    while ((dp = readdir(dir)) != NULL)
    {
    	// 跳过 "." 和 ”..“
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
            continue;
        }
        // 将文件名和路径进行拼接形成一个完整路径
        strcpy(path, basePath);
        strcat(path, "/");
        strcat(path, dp->d_name);
		// 递归遍历
        htfilerecursivedetail(filelist, path, onlyfile);
    }
    closedir(dir);
    return filelist;
}

htlist * htfilerecursive(htlist *filelist, char *basePath) {
    return htfilerecursivedetail(filelist, basePath, 1);
}

htlist * htdirchildren(htlist *filelist, char *basePath) {
    char path[10240];
    struct dirent *dp;
    char *tmpPath;
    DIR *dir;
    int len;

    dir = opendir(basePath);
    while ((dp = readdir(dir)) != NULL)
    {
    	// 跳过 "." 和 ”..“
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
            continue;
        }
        // .开头的文件不在处理范围内
        if (dp->d_name[0] == '.') {
            continue;
        }
        // 将文件名和路径进行拼接形成一个完整路径
        strcpy(path, basePath);
        strcat(path, "/");
        strcat(path, dp->d_name);

        len = strlen(path);
        tmpPath = malloc(len+1);
        strcpy(tmpPath, path);
        tmpPath[len] = '\0';
        htAddNodeUseData(filelist, tmpPath);
    }
    closedir(dir);
    return filelist;
}

