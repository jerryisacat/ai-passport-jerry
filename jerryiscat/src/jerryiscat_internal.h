#pragma once

#include <stddef.h>

#include "jerryiscat_app.h"

// 启动器读取的静态应用注册表。表顺序就是启动器顺序。
size_t jerryiscat_registry_count(void);
const jerryiscat_app_t *jerryiscat_registry_at(size_t index);
