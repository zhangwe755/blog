#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/stat.h>

#include "util.h"

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
    char *dest = malloc(n);
    strncpy(dest, src, n);
    dest[n] = '\0';
    return dest;
}

///////////// file function 
char* getAbsolutePath(char *path) {
    char buf[1024];
    char *abPath;
    size_t pathStart = 0;
    if (path[0] == '/') {
        return path;
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

int isDir(char *file) {
    struct stat buf;
    int result = stat(file, &buf);
    if (result != 0) {
        return 0;
    }
    // 需不需要释放内存
    return S_IFDIR & buf.st_mode;
}

int isFile(char *file) {
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
    if (isFile(filePath)) {
        remove(filePath);
    }
    char *src = malloc(strlen(filePath));
    strcpy(src, filePath);
    filePath = dirname(filePath);
    createDir(filePath);
    FILE *fp = fopen(src, "w");
    return fp;
}


