#include "dict.h"
#include "util.h"


void test_dict_pg() {
    log_debug("run test test_dict_pg");
    htdict *dict = htDictCreate(32);
    htDictPut(dict, "xiaoming", "小明", 1);
    htDictPut(dict, "xiaodong", "晓东", 1);
    char *xiaoming = htDictGet(dict, "xiaoming");
    log_debug("xiaoming的value:%s", xiaoming);

    // 测试循环dict
    hditerator *it = htDictStartIterator(dict);
    htdictentry *entry;
    log_debug("迭代循环");
    while((entry = htDictIteratorNext(it)) != NULL) {
        log_debug("key:%s, value: %s", entry->key , entry->value);
    }


}
