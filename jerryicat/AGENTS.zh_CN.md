<p align="right">
  <strong>简体中文</strong> · <a href="AGENTS.md">English</a>
</p>

# jerryicat 工作区规则

本目录是本 fork 的自有产品层。仓库根 [`AGENTS.md`](../AGENTS.zh_CN.md) 管整个仓库；本文管 `jerryicat/`，每个子目录另有自己的 `AGENTS.md` 说明该目录的细节。最近的 `AGENTS.md` 管其子树，可以覆盖本文；任何层级都不得覆盖根安全基线。

面向人的概览见 [`README.zh_CN.md`](README.zh_CN.md)。

## 这里放什么

- 启动器外壳、共享 UI 套件、玩法、设备端工具、开发侧工具、fork 测试、fork 素材和 fork 文档。
- 建立在上游 BSP 之上的产品体验代码。BSP 本身、上游硬件测试 demo 和上游验证门禁留在原处。

## 构建模型

- 单一固件。启动器、玩法和设备端工具编译进同一个应用；启动器是默认界面。
- `jerryicat/` 是一个名为 `jerryicat` 的 ESP-IDF 组件。`app_main` 只有一个，位于 `main/main.c`；本目录不得再定义第二个。
- 接线：`jerryicat/CMakeLists.txt` 注册组件，仓库根 `CMakeLists.txt` 通过 `EXTRA_COMPONENT_DIRS` 加入它，`main/CMakeLists.txt` 在 `REQUIRES` 中列出 `jerryicat`，`main.c` 调用 `jerryicat_launcher_start()`，固件输入任务把按键事件转给 `jerryicat_handle_key()`。接缝保持最小。
- 除非用户明确改变单固件决定，不得建立独立 ESP-IDF 工程、按玩法拆分的 `sdkconfig` 或重复的分区表。

## 架构

- `include/`——供 `main/` 与同级目录使用的公开头文件：启动器入口、设备端应用接口、共享 UI 套件声明。
- `src/`——启动器外壳、应用注册表、导航、共享 UI 与运行时辅助。
- `plays/`——玩法与体验类应用，每个玩法一个目录。
- `apps/`——设备端工具类应用，每个工具一个目录。
- `tools/`——开发侧脚本；不放固件源码。
- `tests/`——fork 逻辑的 host tests；不依赖 ESP-IDF 或 LVGL。
- `assets/`——fork 专用字体、图片与音频，含来源记录。
- `docs/`——fork 架构说明、玩法规格与产品决策。

## 设备端应用契约

玩法与设备端工具共用 `include/` 中声明的同一套生命周期接口，只在启动器分类上不同。

- 每个应用由 id（唯一 lowercase-kebab，与目录名一致）、启动器标题、类别（`play` 或 `tool`）以及生命周期回调描述：`enter`、`exit`、按键处理，以及异步工作可选的 `start`/`stop`。
- `enter` 建立应用页面并重置状态；`exit` 在全部工作停止后删除这些页面。`start`/`stop` 包住工作任务、音频、网络和持久化——这些不应在应用隐藏时继续运行。
- 按键回调只派发轻量事件。启动器保留 `OK` 长按作为返回手势；应用负责短按行为和额外手势，不照抄 demo 菜单。
- 在启动器表中注册应用。未注册的应用就是死代码；构建不得悄悄把它带进来。
- 每个应用拥有自己的视觉语言。基线 demo 菜单、`demo_*.c` 页面和 `ui_pixel` 外壳只作参考，绝不作起点。

## 运行时规则

- 非 LVGL 任务访问 LVGL 对象时必须持有 `bsp_lvgl_lock()`。
- 删除应用页面前、或离开应用前，停止所有可能触达其 UI 的任务、定时器、回调和事件处理器；把音频、网络和存储资源恢复到空闲状态。
- 默认在右上角通过 `bsp_battery_soc()` 显示电量；读值为 `-1` 时不绘制数字，并避开应用自身内容。
- 屏幕空闲时调暗或关闭背光；应用流程允许时回到 light sleep 或 deep sleep。
- 精打细算内部 RAM：无 PSRAM，每个玩法不得占用大块静态缓冲。按需加载字体和图片，应用不再需要时释放。
- 中文 UI 属于字体集成任务。发布任何中文文案前，先完成根目录的字体检查清单。

## 目录地图

| 目录 | 用途 | 本地规则 |
| --- | --- | --- |
| [`include/`](include/AGENTS.zh_CN.md) | 启动器与设备端应用接口的公开头文件 | `include/AGENTS.zh_CN.md` |
| [`src/`](src/AGENTS.zh_CN.md) | 启动器外壳、注册表、共享 UI 套件 | `src/AGENTS.zh_CN.md` |
| [`plays/`](plays/AGENTS.zh_CN.md) | 玩法与体验类应用 | `plays/AGENTS.zh_CN.md` |
| [`apps/`](apps/AGENTS.zh_CN.md) | 设备端工具类应用 | `apps/AGENTS.zh_CN.md` |
| [`tools/`](tools/AGENTS.zh_CN.md) | 开发侧脚本 | `tools/AGENTS.zh_CN.md` |
| [`tests/`](tests/AGENTS.zh_CN.md) | fork 逻辑的 host tests | `tests/AGENTS.zh_CN.md` |
| [`assets/`](assets/AGENTS.zh_CN.md) | fork 字体、图片与音频 | `assets/AGENTS.zh_CN.md` |
| [`docs/`](docs/AGENTS.zh_CN.md) | fork 设计与产品文档 | `docs/AGENTS.zh_CN.md` |

## 新增玩法或设备端工具

1. 先读对应目录规则（`plays/AGENTS.zh_CN.md` 或 `apps/AGENTS.zh_CN.md`）和 `include/` 中的接口头文件。
2. 用唯一 lowercase-kebab 名称建立目录，并在其中加入自己的 `AGENTS.md` 与 `AGENTS.zh_CN.md`，说明用途、页面、操作、状态、持久化、任务、退出清理、测试和真机检查。
3. 实现共享生命周期接口，并在启动器表中注册应用。
4. 把源码加入 `jerryicat/CMakeLists.txt`，为纯逻辑添加 host tests，并接入 `jerryicat/tools/validate.sh`。
5. 更新 [`README.zh_CN.md`](README.zh_CN.md) 与 `docs/` 中的相关说明。
6. 运行上游门禁与 fork 测试，主动询问真机测试，然后按根目录维护规则在 `main` 上 commit 并 push。

## 当前状态

启动器和第一个玩法 `+1` 已实现并接入单一固件。新玩法和设备端工具沿用同一套接口、
注册表、测试与目录 `AGENTS.md` 规则。
