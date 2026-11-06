#include "jerryiscat_nav.h"

int jerryiscat_nav_next(int selected, int delta, int count) {
    if (count <= 0) return -1;
    if (selected < 0 || selected >= count) selected = 0;

    // 先对 delta 取模，避免大增量溢出；负数余数在最后统一补回正区间。
    int next = (selected + delta % count) % count;
    if (next < 0) next += count;
    return next;
}
