#include "dict.h"

#ifndef HT_CONFIG
#define HT_CONFIG

typedef struct ht_config_entity {
    int inited;
    char *run_dir;
    char *config_file;
    char *watch_dir;
    char *root_dir;
    char *src_dir;
    char *dest_dir;
    htdict *argdict;

} htconfigentity;

htconfigentity htconfig;

#endif

void ht_config_init(int argc, char **argv);
char *ht_config_get(char *key);


