#pragma once

#include <stddef.h>

#include "jerryicat_app.h"

// 启动器读取的静态应用注册表。表顺序就是启动器顺序。
size_t jerryicat_registry_count(void);
const jerryicat_app_t *jerryicat_registry_at(size_t index);
