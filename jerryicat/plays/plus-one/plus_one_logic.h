#pragma once

#include <stdint.h>

// 每次按键把计数加一；到达 uint32_t 上限后保持不变，避免回绕成 0。
static inline uint32_t plus_one_increment(uint32_t count) {
    return count == UINT32_MAX ? count : count + 1u;
}
