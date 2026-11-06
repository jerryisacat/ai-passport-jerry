<p align="right">
  <strong>简体中文</strong> · <a href="AGENTS.md">English</a>
</p>

# tests/ 规则

本目录放 fork 逻辑的 host tests。它们只在开发机上运行，不在设备上运行，也不需要 ESP-IDF 或 LVGL。

## 范围

- 测试纯逻辑：启动器导航与选中、应用状态机、计分、注入时钟的计时、布局计算、解析，以及 NVS 值编解码辅助。
- 不在这里测试 LVGL 渲染、FreeRTOS 调度、硬件驱动或真实音频。这些需要固件构建与真机检查。

## 约定

- C 测试：`test_<module>.c`，C11，使用 `-std=c11 -Wall -Wextra -Werror` 编译。只链接被测模块，必要时链接 `tests/stubs/` 中的小桩。
- Python 测试：`test_<module>.py`，仅标准库，可直接运行。
- 一个测试验证一个行为，使用直接的断言和注明预期结果的错误消息。测试失败路径、边界值和重置行为，而不是私有实现细节。
- 不 sleep、不联网、不碰硬件；随机数必须固定种子。
- 保持测试足够快，能在每次改动时运行。

## 接入与验证

- 与被测代码同一轮添加或更新测试。
- 把每个测试接入 [`../tools/validate.sh`](../tools/validate.sh)，交付前运行该脚本。
- Host test 结果要与固件构建、真机结果分开报告。host test 通过不能证明屏幕、音频或功耗行为。

## 命名

- 按被测模块分组，而不是按第一个用到它的应用分组。
- 共享桩放在 `tests/stubs/`，保持最小、有注释，且不含产品逻辑。
