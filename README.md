# jerryiscat 固件

这是给 **FoloToy AI Passport**（ESP32-C3，8 MB Flash，无 PSRAM）开发的固件。它基于上游
[`FoloToy/ai-passport`](https://github.com/FoloToy/ai-passport) 的 BSP，采用单一固件 + 自研启动器：
开机先看到启动器，再进入固件里的各个工具或玩法。

> 这份根目录 `README.md` 是中文单语版本；所有 `README.md` 都只维护中文，
> `AGENTS.md` 只维护英文；其余文档保持英文 `.md` + 中文 `.zh_CN.md` 的配对规则。

## 固件

**jerryiscat** —— 当前唯一的固件，编译进一个 ESP-IDF 应用：

- 启动器：应用列表、选中、电量显示、按键导航
- 四个硬件工具：Wi-Fi 扫描、BLE 广播、麦克风电平、扬声器提示音
- 目前还没有玩法（`plays/` 为空，之前的 `+1` 已移除）

## 固件中的程序

| 程序 | 类型 | 做什么 |
| --- | --- | --- |
| 启动器 | 系统 | 开机默认界面；`UP`/`DOWN` 选择，`OK` 进入，`OK` 长按从工具返回 |
| Wi-Fi Scan | 硬件工具 | 扫描 2.4 GHz AP，显示 SSID、RSSI、信道；只扫描，不连接、不存凭据 |
| BLE Beacon | 硬件工具 | 以 `jerryiscat` 为名做不可连接 BLE 广播；无配对、无 GATT |
| Mic Level | 硬件工具 | 16 kHz / 16 bit / 单声道读取麦克风，显示峰值电平；不录音、不保存 |
| Speaker Tone | 硬件工具 | 以 40% 音量播放 2 秒 500 Hz 方波，用来自检扬声器 |

源码入口：

- 启动器：[jerryiscat/src/jerryiscat_launcher.c](jerryiscat/src/jerryiscat_launcher.c)
- 应用注册表：[jerryiscat/src/jerryiscat_registry.c](jerryiscat/src/jerryiscat_registry.c)
- 四个工具：[jerryiscat/apps/](jerryiscat/apps/)

## 固件索引

除本仓库的 `jerryiscat` 外，同一块硬件上还验证过社区固件 **DSH Passport**（BLE 任务终端，
配套 DSH 插件）。两份镜像的来源、commit、校验值、能力与刷写/回退方式记录在
[jerryiscat/docs/firmware-index.zh_CN.md](jerryiscat/docs/firmware-index.zh_CN.md)。

## 硬件支持

| 能力 | 实现方式 | 说明 |
| --- | --- | --- |
| 显示 | ST7789P3，240×320 竖屏，SPI | 启动器与所有工具共用 `jerryiscat_ui` 的暗色样式 |
| 按键 | `UP` / `DOWN` / `OK` 共用 ADC 分压 | 按键回调只入队；`OK` 长按由启动器保留为返回手势 |
| 电量 | CW2017，共享 I2C | 右上角显示百分比；读不到时留空 |
| Wi-Fi | ESP32-C3 2.4 GHz 802.11 b/g/n | 当前只做扫描；不连接、不配网 |
| 蓝牙 | Bluetooth 5 LE（NimBLE） | **不支持经典蓝牙**；当前只做非连接广播 |
| 麦克风 | ES8311 ADC，I2S `DIN=GPIO4` | 单声道，当前 30 dB PGA 增益 |
| 扬声器 | ES8311 DAC，I2S `DOUT=GPIO2` | 外部功放常通，软件无法关闭其待机电流 |

## 内存与代码大小

- 只有 8 MB Flash、约 400 KB SRAM，**没有 PSRAM**。
- 所有音频任务只用 160 采样（320 字节）的栈缓冲；不分配整段录音缓冲。
- Wi-Fi 和 NimBLE 栈只在对应工具打开期间运行，离开即释放。
- 关闭了仅供 `+1` 使用的 Montserrat 48 字体；只保留 14/20 号。
- 上游 demo 源码仍在 `main/` 中作为参考，但已不再编译进固件。

## 构建与烧录

需要 ESP-IDF 5.5.3：

```bash
source ~/esp/esp-idf-v5.5.3/export.sh
cd /Users/sunrunchen/GitHub/ai-passport-jerry
idf.py build
idf.py -p /dev/cu.usbmodem101 flash monitor
```

仓库级验证：

```bash
./tools/validate.sh            # 静态检查 + host tests + 固件构建
./jerryiscat/tools/validate.sh # fork host tests
```

## 项目结构

```text
README.md                 本文件（中文单语，给人看）
jerryiscat/               本 fork 的产品层：启动器、工具、测试、文档
  include/                启动器与设备端应用接口
  src/                    启动器、注册表、共享 UI、radio 辅助
  apps/                   四个硬件工具
  plays/                  玩法目录（当前为空）
  tests/                  host tests
  tools/                  开发侧脚本
main/                     固件入口与上游 demo 源码（demo 不参与编译）
components/bsp/           上游板级支持包
docs/                     上游与 fork 维护文档
```

## 当前状态

- 启动器、Wi-Fi 扫描、BLE 广播、麦克风电平、扬声器提示音均已实现。
- 固件构建与 shell 门禁见 [`jerryiscat/README.md`](jerryiscat/README.md)。
- 玩法区为空；下一步可以在这个骨架上加真正的玩法。
