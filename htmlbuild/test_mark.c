#include "util.h"
#include "file_mark.h"

void test_mark_str() {
    /** * 测试 **/
    {
        char *starSrc = "*xax*";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "**xax**";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "***xax***";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "***xax**";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "**xax***";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "*";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "*小明*";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
}


void test_mark_title() {
    {
        char *starSrc = "#小明*";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "##小明*";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "###小明*";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "####小明*";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "#####小明*";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "######小明*";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "######小明*";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "\n######小明*";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "\r######小明*";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "\r######小明*\r***小东**";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
}

void test_mark_ref() {
    {
        char *starSrc = ">注意这是个大瓜";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = ">######小明*";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "\r>######小明*";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "\r>#####*#小明*";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
}

void test_mark_code() {
    {
        char *starSrc = "```l#####*#小明*```";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "zhang```l#####*#小明*```";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "zhang后面换行```\nl#####*#小明*```";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "zhang```";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
}

void test_mark_link() {
    {
        char *starSrc = "[测试文本](http://baidu.com)";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "测试文本](http://baidu.com)";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "[测试文本](http://baidu.com)";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }

}

void test_mark_img() {
    {
        char *starSrc = "![测试文本](http://baidu.com)";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "测试文本](http://baidu.com)";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    {
        char *starSrc = "![测试文本](http://baidu.com)(width:20px)";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
}
