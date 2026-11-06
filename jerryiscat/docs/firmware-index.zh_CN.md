<p align="right">
  <strong>简体中文</strong> · <a href="firmware-index.md">English</a>
</p>

# 固件索引

本页索引本 fork 为 AI Passport 板子构建或验证过的固件镜像，记录身份、来源与取舍；
它不是构建教程。本 fork 自己的镜像由 `jerryiscat/` 构建；社区镜像是独立项目，
有各自的仓库和许可证，收录在这里不代表它属于本 fork 的构建。本页只记录来源，
不内置社区镜像；如果 release 消失，按固定的 commit 重新构建。

## 镜像

| 镜像 | 来源 | 身份 | 刷写 | 状态 |
| --- | --- | --- | --- | --- |
| jerryiscat | 本 fork | commit `422c44d`；合并镜像 1,572,528 字节；SHA-256 `a578502a…94bb58` | `0x0` 整片镜像 | 2026-11-06 构建；社区镜像刷入后不在设备上 |
| DSH Passport | `slinxiaosun-blip/ai-passport-dsh` | `v1.3.1-dsh-passport`，commit `57e4aed`；1,818,528 字节；SHA-256 `023cca4f…9b26` | `0x0` 整片镜像 | 2026-11-07 刷入；BLE 与电脑端连接已验证；语音待验 |

## jerryiscat

本 fork 的单一固件：启动器、Wi-Fi Scan、BLE Beacon、Mic Level、Speaker Tone
编译进同一个 ESP-IDF 应用，在上游 BSP 之上使用本 fork 自己的界面。

- 构建：ESP-IDF 5.5.3 下 `idf.py build`；合并产物
  `build/FoloToy-AI-Passport-full.bin`。
- 体积：应用 1,506,992 字节；合并镜像 1,572,528 字节；factory 分区剩余 82%。
  构建产物的 SHA-256 为
  `a578502a8552c6a7e5ef256770098f0dfd253e5c49f5ca8a735d2e7e4394bb58`。
- 刷写：`0x0` 整片镜像；会替换整个应用，并可能重置 NVS。
- 状态：2026-11-06 构建与 host 检查通过。设备当前运行 DSH Passport，因此这份镜像
  不在设备上。

## DSH Passport

社区固件，把板子变成桌面 DSH 会话的 BLE 终端。它有自己的设备固件和配套 DSH 插件，
不属于本 fork 的单一固件构建。

- 用途：聚合任务状态、完成横幅与提示音、工具审批、余额、语音输入，以及设置/配对页。
- 链路：BLE GATT，自定义帧 + JSON 载荷，协议 v1；不需要 Wi-Fi。BLE 名称为
  `FoloPassport-DSH`。配对使用每次上电都会变的 6 位码。
- 主机：DSH 桌面版 0.2.1-rc.5；配套插件 `dsh-ai-passport-plugin` 1.3.1
  （commit `3b2daee`）。插件支持 macOS Apple Silicon 与 Windows 10+。
- 语音：设备负责录音，转写需要本地 SenseVoice provider（约 240 MB 模型，默认未启用）。
  没有它时主机会回「语音未就绪」。
- 界面：当前固件是精简状态卡，显示状态、计时和当前工具，没有任务列表。任务与文本输入
  仍留在电脑上。
- 审批：高风险工具默认停在「拒绝」；倒计时归零按拒绝处理。
- 约束：从 `0x0` 整片刷入会清掉 NVS，配对码与信任状态随之丢失。刷入后设备运行
  DSH Passport，直到重新刷回 jerryiscat。
- 许可：固件 MIT，基于 `FoloToy/ai-passport`（MIT）；配套插件 MIT。

## 切换镜像

两份都是写在 `0x0` 的完整合并镜像：

```bash
esptool.py --chip esp32c3 -p /dev/cu.usbmodemXXXX -b 460800 \
  write_flash --flash_mode dio --flash_size 8MB --flash_freq 80m \
  0x0 <image.bin>
```

整片刷写会清掉 NVS，配对状态与已存设置不保留。要回到 jerryiscat，
刷入本仓库构建的 `build/FoloToy-AI-Passport-full.bin`。

## 验证记录

- 2026-11-06：jerryiscat 构建 PASS；应用 1,506,992 字节，合并镜像 1,572,528 字节。
- 2026-11-07：DSH Passport v1.3.1 从 `0x0` 刷入；esptool 报告
  `Hash of data verified`；观测到 BLE 广播 `FoloPassport-DSH`，RSSI −41 dBm；
  电脑端插件已连接（用户确认）。语音转写尚未验证。
- 推断而非观测：82% 剩余来自分区表与合并镜像大小，不是设备端实测。

## 来源

- 固件：<https://github.com/slinxiaosun-blip/ai-passport-dsh>，tag
  `v1.3.1-dsh-passport`，commit
  `57e4aed4d50934d423c65b99f20017252724ca17`。
- 插件：<https://github.com/slinxiaosun-blip/dsh-ai-passport-plugin>，版本 1.3.1，
  commit `3b2daee4e213c60be1b1460a75ec34778607bd2c`。
- 镜像：`FoloToy-AI-Passport-full.bin`，1,818,528 字节，SHA-256
  `023cca4fbe9aebd38e9ee775f1479bc97720d9947e8705ecb9accbe138029b26`。
