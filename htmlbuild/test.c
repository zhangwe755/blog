#include <string.h>
#include "util.h"

#include "hello.h"
#include "test_util.h"
#include "test_mark.h"

int main(int argc, char **argv) {
    log_info("argc:%d", argc);
    if (argc < 2) {
        log_error("请指定测试方法, 指定方式运行:./test funciton_name");
        return 0;
    }
    char *function_name = argv[1];
    log_info("function name:%s", function_name);
    open_debug();
    if (strncmp("test_", function_name, 5) != 0) {
        log_error("测试方法名必须是test_开头的函数");
        return 0;
    }
    if (strcmp(function_name, "test_hello") == 0) {
        test_hello();
    }
    if (strcmp(function_name, "test_ht_read_file") == 0) {
        test_ht_read_file();
    }
    if (strcmp(function_name, "test_build_mark_str") == 0) {
        test_build_mark_str();
    }

    return 0;
}

