#include "util.h"
#include "file_mark.h"

void test_build_mark_str() {
    /** * 测试 **/
    /*
    {
        char *starSrc = "*xax*";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    */
    /*
    {
        char *starSrc = "**xax**";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    */
    /*
    {
        char *starSrc = "***xax***";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    */
    /*
    {
        char *starSrc = "***xax**";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    */

    /*
    {
        char *starSrc = "**xax***";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    */
    /*
    {
        char *starSrc = "*";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    */
    /*
    {
        char *starSrc = "*小明*";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }
    */

    /** # 测试 **/
    /*
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
    */
    /*** 引用测试 ***/
    /*
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
    */
    /*** 代码块测试 ***/
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
        char *starSrc = "zhang```\nl#####*#小明*```";
        log_info("star src:%s", starSrc);
        char *starDest = build_mark_str(starSrc);
        log_debug("star dest:%s", starDest);
    }

}
