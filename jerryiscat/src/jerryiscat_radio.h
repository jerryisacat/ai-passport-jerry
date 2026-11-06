#pragma once

#include "esp_err.h"

// NVS 是 Wi-Fi/NimBLE 的前置依赖。失败时不擦除分区，避免丢掉用户数据。
esp_err_t jerryiscat_radio_nvs_prepare(void);

// Wi-Fi 默认 STA netif 依赖 netif 与默认事件循环；只初始化一次并保留。
esp_err_t jerryiscat_radio_netif_prepare(void);
