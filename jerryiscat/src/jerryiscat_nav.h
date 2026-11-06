#pragma once

// 选中项在 [0, count) 内循环移动。
// count <= 0 时返回 -1；selected 越界时先归一到 0 再移动。
int jerryiscat_nav_next(int selected, int delta, int count);
