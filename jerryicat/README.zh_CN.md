<p align="right">
  <strong>简体中文</strong> · <a href="README.md">English</a>
</p>

# jerryicat

本目录是本 fork 在 FoloToy AI Passport 基线之上的自有产品层。上游 BSP、基线 demo、工程参考和验证门禁仍是基础；本 fork 自研的一切——启动器、玩法、设备端工具、开发侧工具、素材和文档——都放在这里。

面向 AI agent 的规则见 [`AGENTS.zh_CN.md`](AGENTS.zh_CN.md)。每个子目录另有自己的 `AGENTS.zh_CN.md`，说明该目录的详细工作规则。

## 当前状态

启动器和第一个玩法 `+1` 已实现并接入单一固件。新应用沿用同一套接口、注册表、
测试与目录 `AGENTS.md` 规则。

## 应用

| 应用 | 目录 | 行为 | 规则 |
| --- | --- | --- | --- |
| `+1` | `plays/plus-one/` | 任意按键按下加一；长按 `OK` 返回启动器 | [`AGENTS.zh_CN.md`](plays/plus-one/AGENTS.zh_CN.md) |

## 目录

| 路径 | 用途 | 规则 |
| --- | --- | --- |
| `include/` | 启动器与设备端应用接口的公开头文件 | [`include/AGENTS.zh_CN.md`](include/AGENTS.zh_CN.md) |
| `src/` | 启动器外壳、应用注册表、共享 UI 套件 | [`src/AGENTS.zh_CN.md`](src/AGENTS.zh_CN.md) |
| `plays/` | 玩法与体验类应用，每个玩法一个目录 | [`plays/AGENTS.zh_CN.md`](plays/AGENTS.zh_CN.md) |
| `apps/` | 设备端工具类应用，每个工具一个目录 | [`apps/AGENTS.zh_CN.md`](apps/AGENTS.zh_CN.md) |
| `tools/` | 开发侧脚本 | [`tools/AGENTS.zh_CN.md`](tools/AGENTS.zh_CN.md) |
| `tests/` | fork 逻辑的 host tests | [`tests/AGENTS.zh_CN.md`](tests/AGENTS.zh_CN.md) |
| `assets/` | fork 字体、图片与音频及来源记录 | [`assets/AGENTS.zh_CN.md`](assets/AGENTS.zh_CN.md) |
| `docs/` | fork 设计说明与产品文档 | [`docs/AGENTS.zh_CN.md`](docs/AGENTS.zh_CN.md) |

## 产品模型

- 设备使用单一固件。启动器、所有玩法和所有设备端工具编译进同一个应用；启动器是用户首先看到的界面。
- 玩法与设备端工具共用一套生命周期接口：enter、exit、按键处理，以及异步工作可选的 start/stop。
- 应用拥有自己的页面、操作和视觉设计。上游 demo 菜单与 `ui_pixel` 外壳只作参考。
- 开发侧工具和 host tests 不进入固件，分别放在 `tools/` 和 `tests/`。

## 在这里工作

1. 先读仓库根 [`AGENTS.zh_CN.md`](../AGENTS.zh_CN.md) 和本工作区的 [`AGENTS.zh_CN.md`](AGENTS.zh_CN.md)。
2. 读将要修改的目录的 `AGENTS.zh_CN.md`。
3. 实现改动，同时补齐测试与文档配对。
4. 运行上游门禁（`./tools/validate.sh`）以及 fork host tests（`./jerryicat/tools/validate.sh`）。
5. 固件改动主动询问真机测试，然后按根目录维护规则在 `main` 上 commit 并 push。

仓库模型见 [`docs/fork-guide.zh_CN.md`](../docs/fork-guide.zh_CN.md)。
