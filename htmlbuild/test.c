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
        return 0;
    }
    if (strcmp(function_name, "test_ht_read_file") == 0) {
        test_ht_read_file();
        return 0;
    }
    if (strcmp(function_name, "test_mark_str") == 0) {
        test_mark_str();
        return 0;
    }
    if (strcmp(function_name, "test_mark_title") == 0) {
        test_mark_title();
        return 0;
    }
    if (strcmp(function_name, "test_mark_ref") == 0) {
        test_mark_ref();
        return 0;
    }
    if (strcmp(function_name, "test_mark_code") == 0) {
        test_mark_code();
        return 0;
    }
    if (strcmp(function_name, "test_mark_link") == 0) {
        test_mark_link();
        return 0;
    }
    log_info("function name:%s, not find test", function_name);
    return 0;
}

