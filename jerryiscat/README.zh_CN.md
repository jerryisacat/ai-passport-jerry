<p align="right">
  <strong>简体中文</strong> · <a href="README.md">English</a>
</p>

# jerryiscat

本目录是本 fork 在 FoloToy AI Passport 基线之上的自有产品层。上游 BSP、基线 demo、工程参考和验证门禁仍是基础；本 fork 自研的一切——启动器、玩法、设备端工具、开发侧工具、素材和文档——都放在这里。

面向 AI agent 的规则见 [`AGENTS.md`](AGENTS.md)。每个子目录另有自己的 `AGENTS.md`，说明该目录的详细工作规则。

## 当前状态

启动器和四个硬件工具已实现并接入单一固件。目前还没有玩法；新应用沿用同一套接口、
注册表、测试与目录 `AGENTS.md` 规则。

## 应用

| 应用 | 目录 | 行为 | 规则 |
| --- | --- | --- | --- |
| Wi-Fi Scan | `apps/wifi-scan/` | 扫描 2.4 GHz AP，显示 SSID、RSSI、信道 | [`AGENTS.md`](apps/wifi-scan/AGENTS.md) |
| BLE Beacon | `apps/ble-beacon/` | 以 `jerryiscat` 名称做不可连接 BLE 广播 | [`AGENTS.md`](apps/ble-beacon/AGENTS.md) |
| Mic Level | `apps/mic-level/` | 实时麦克风峰值电平 | [`AGENTS.md`](apps/mic-level/AGENTS.md) |
| Speaker Tone | `apps/speaker-tone/` | 两秒 500 Hz 扬声器自检 | [`AGENTS.md`](apps/speaker-tone/AGENTS.md) |

## 固件索引

本板构建或验证过的镜像，含固定的身份与取舍：
[`docs/firmware-index.zh_CN.md`](docs/firmware-index.zh_CN.md)。

## 目录

| 路径 | 用途 | 规则 |
| --- | --- | --- |
| `include/` | 启动器与设备端应用接口的公开头文件 | [`include/AGENTS.md`](include/AGENTS.md) |
| `src/` | 启动器外壳、应用注册表、共享 UI 套件 | [`src/AGENTS.md`](src/AGENTS.md) |
| `plays/` | 玩法与体验类应用，每个玩法一个目录 | [`plays/AGENTS.md`](plays/AGENTS.md) |
| `apps/` | 设备端工具类应用，每个工具一个目录 | [`apps/AGENTS.md`](apps/AGENTS.md) |
| `tools/` | 开发侧脚本 | [`tools/AGENTS.md`](tools/AGENTS.md) |
| `tests/` | fork 逻辑的 host tests | [`tests/AGENTS.md`](tests/AGENTS.md) |
| `assets/` | fork 字体、图片与音频及来源记录 | [`assets/AGENTS.md`](assets/AGENTS.md) |
| `docs/` | fork 设计说明与产品文档 | [`docs/AGENTS.md`](docs/AGENTS.md) |

## 产品模型

- 设备使用单一固件。启动器、所有玩法和所有设备端工具编译进同一个应用；启动器是用户首先看到的界面。
- 玩法与设备端工具共用一套生命周期接口：enter、exit、按键处理，以及异步工作可选的 start/stop。
- 应用拥有自己的页面、操作和视觉设计。上游 demo 菜单与 `ui_pixel` 外壳只作参考。
- 开发侧工具和 host tests 不进入固件，分别放在 `tools/` 和 `tests/`。

## 在这里工作

1. 先读仓库根 [`AGENTS.md`](../AGENTS.md) 和本工作区的 [`AGENTS.md`](AGENTS.md)。
2. 读将要修改的目录的 `AGENTS.md`。
3. 实现改动，同时补齐测试与文档配对。
4. 运行上游门禁（`./tools/validate.sh`）以及 fork host tests（`./jerryiscat/tools/validate.sh`）。
5. 固件改动主动询问真机测试，然后按根目录维护规则在 `main` 上 commit 并 push。

仓库模型见 [`docs/fork-guide.zh_CN.md`](../docs/fork-guide.zh_CN.md)。
