#include "util.h"
#include "md5.h"
#include "test_md5.h"

void test_calc_md5() {
    log_debug("test calc md5");
    char dest[32] = {0};
    calc_md5("./util.h", dest);
    log_debug("test md5:%s", dest);
}
