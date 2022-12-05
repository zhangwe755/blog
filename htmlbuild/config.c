#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "dict.h"
#include "util.h"

char *_config_key_list[] = {"run_dir","watch_dir","root_dir", "dest_dir", "config_file"};
int _config_key_list_len = sizeof(_config_key_list)/sizeof(char *);

char *ht_config_get(char *key) {
    return htDictGet(htconfig.argdict, key);
}

int config_is_key(char *key) {
    for (int i=0;i<_config_key_list_len;i++) {
        if (strcmp(_config_key_list[i], key)==0) {
            return 1;
        }
    }
    return 0;
}

void config_load_config(char *argitem) {
    int len = strlen(argitem);
    int ecindex = -1;
    char tmpchar;
    for (int i=0;i<len;i++) {
        tmpchar = argitem[i];
        if(tmpchar == '=') {
            ecindex = i;
            break;
        }
    }
    if (ecindex <= 0) {
        return;
    }
    char *key = malloc(ecindex+1);
    char *value = malloc(len-ecindex);
    strncpy(key, argitem, ecindex);
    key[ecindex] = '\0';
    if (!config_is_key(key)) {
        return;
    }
    strncpy(value, argitem+ecindex+1, len-ecindex-1);
    value[len-ecindex-1] = '\0';
    htDictPut(htconfig.argdict, key, value, 0);
}

char *config_get_config_file() {
    if (htconfig.config_file != NULL) {
        return htconfig.config_file;
    }
    char *configfile = ht_config_get("config_file");
    if (configfile == NULL) {
        // 设置默认配置文件
        configfile = htContactTwoStr(htconfig.run_dir , "/.config_file");
        printf("=>use deafult config file:%s\n", configfile);
    }
    if (configfile != NULL) {
        if (!isFile(configfile)) {
            printf("=>not find config file! the config file path:%s\n", configfile);
            configfile = NULL;
        }
    }
    return configfile;
}

void config_run_dir_init() {
    char *runDir = currentDir();
    htDictPut(htconfig.argdict, "run_dir", runDir, 0);
    htconfig.run_dir = runDir;
}

void config_file_init() {
    char *configfile = config_get_config_file();
    if (configfile == NULL) {
        return;
    }
    htconfig.config_file = configfile;
    FILE *fd = fopen(configfile, "r");
    if (!fd) {
        printf("config file read fail! the config file path:%s\n", configfile);
        return;
    }
    char *line = malloc(1024);
    do {
        line = fgets(line, 1024, fd);
        if (line == NULL) {
            break;
        }
        config_load_config(line);
    } while(line != NULL);
    fclose(fd);
    free(line);
}

void config_arg_init(int argc, char **argv) {
    for (int i=0;i<argc;i++) {
        config_load_config(argv[i]);
    }
}

void printConfig() {
    for (int i=0;i<_config_key_list_len;i++) {
        char *key = _config_key_list[i];
        printf("=>config: %s:%s\n", key, ht_config_get("root_dir"));
    }
}

void ht_config_init(int argc, char **argv) {
    if (htconfig.inited) {
        return;
    }
    printf("=>config init start!\n");
    htconfig.argdict = htDictCreate(32);
    htconfig.config_file = NULL;
    htconfig.root_dir = NULL;
    htconfig.dest_dir = NULL;
    htconfig.inited = 1;
    config_run_dir_init();
    config_arg_init(argc, argv);
    config_file_init();

    // 快捷字段设置
    htconfig.root_dir = ht_config_get("root_dir");
    htconfig.dest_dir = ht_config_get("dest_dir");
    htconfig.watch_dir = ht_config_get("watch_dir");
    printConfig();
    printf("=>config init finish!\n");
}

