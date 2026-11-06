<p align="right">
  <strong>简体中文</strong> · <a href="AGENTS.md">English</a>
</p>

# AI Agent 仓库规范

本文是本仓库 AI 辅助工作的唯一必读入口，约束整个仓库。根据下方路由表读取当前任务所需文档，不要默认加载全部 README。

## 仓库定位

本仓库是从 [`FoloToy/ai-passport`](https://github.com/FoloToy/ai-passport) 直接复制而来的下游仓库，对应远程 `upstream`。上游提供硬件契约、BSP、基线演示固件、工程参考、AI 技能和验证工具；这些内容是开发基础和硬件事实来源。

本 fork 自主开发的一切——启动器、玩法、设备端工具、开发侧工具、fork 素材和 fork 文档——都放在 [`jerryicat/`](jerryicat/AGENTS.zh_CN.md) 下。不得把第一方产品代码散落到 `main/`、`components/`、`tools/` 或 `tests/`。

`jerryicat/` 下的每个文件夹都有自己的 `AGENTS.md`，说明该目录的用途和工作方式。修改任何目录前，先读 `jerryicat/AGENTS.md` 和该目录的 `AGENTS.md`。最近的 `AGENTS.md` 管其子树，可以补充或覆盖更上层的说明，但不得覆盖本文的安全基线。

## 仓库维护要求

- 所有开发和文档工作都直接在当前检出的 `main` 分支上进行。除非用户明确要求，不创建分支、worktree 或 Pull Request。
- 每轮开发或文档工作完成后，提交本轮改动并推送到 `origin/main`。本规则即为这些 commit 和 push 的长期授权，不需要每轮另行等待用户确认。
- 提交前审阅完整 diff，只暂存属于本轮的文件；不得把无关的用户改动或未跟踪文件卷进 commit。
- 提交前运行适用的检查。已知失败不得推送；先修复，或报告阻碍。
- 遵循 `docs/contribution/commit-and-pr.zh_CN.md` 的 Conventional Commit 格式：英文祈使句标题，例如 `docs(agents): record the fork workspace model`。
- 只推送到 `origin`。不得推送到 `upstream`、强制推送、改写已发布历史或移动 tag。
- 自动上游同步已关闭。只有用户要求时才从 `upstream` fetch 或 merge；冲突以本文和 `jerryicat/` 规则为准解决，不得为了采用上游版本而整体丢弃 fork 工作。
- 上游文档中要求 `main` 与上游保持同步、或在 `feature/*` 分支开发的说法，以本节为准。

## 必需 AI 技能

以下五个技能是本仓库 AI 辅助开发的必需项：`passport-develop`、
`passport-setup`、`passport-build`、`passport-device-test`、`passport-debug`。
技能权威源文件位于 `skills/`。

开始开发前，AI 必须自行检查这五个技能是否已在当前环境安装并可用。缺少时，
由 AI 根据所用工具和环境，自行选择合适的安装方式、完成安装并验证可用性。
不等待用户另外提出安装要求，也不把选择安装方式或执行安装命令交给用户。
不强制使用特定脚本、安装目录或 AI 客户端。

安装必须遵守环境的权限审批要求，保留已有技能和用户配置。权限、冲突或平台
限制导致无法安装时，说明阻碍，只请求必要的用户配合，不得谎报安装成功。
五个技能必须可用，不代表每次任务都加载或调用全部技能；只使用匹配当前任务
的技能。烧录和发布仍须分别取得授权；Git 写操作按上方维护要求执行。

这些技能描述的是上游开发流程；其环境准备、构建、真机测试和调试阶段仍然适用，但应用代码放置的默认约定以 `jerryicat/` 规则为准。

## 项目与安全基线

- 目标平台：ESP32-C3、8 MB Flash、无 PSRAM、ESP-IDF 5.5.3。
- 仓库默认分区表保持最简：只包含 NVS、PHY data，以及占用 8 MB Flash
  剩余空间的单个 factory app。用户固件可以按需求明确调整布局；修改后必须
  验证结果，不得把产品专用分区变成模板的强制契约。
- 保留用户已有修改。先执行 `git status --short --branch`，不得覆盖或清理无关文件。
- 下载（烧录）新固件无需备份设备内原有固件，不得把读取 Flash 备份作为前置条件。这不保证保留用户数据，也不授权全片擦除；遵循[烧录与数据说明](docs/development/engineering/firmware-layout.zh_CN.md#烧录与已存数据)。
- 硬件事实优先级：产品规格与实测结果 → `components/bsp/include/bsp_pins.h` → BSP 头文件与实现 → 硬件指南 → README/demo。任务所需硬件细节未在这些来源中定义时，直接询问用户，不得猜测。
- 可复用板级逻辑放入 `components/bsp`。启动器、共享 UI 套件、玩法、设备端工具及其任务放入 `jerryicat/`。`main/` 保留为固件入口和上游硬件测试 demo；产品逻辑不得在其中增长。
- 二次开发应用必须重新设计并实现独立 UI，禁止沿用当前 demo 测试菜单、页面或界面外壳；仅改名、换颜色或在原界面增加功能不算重新设计。BSP API 和非 UI 逻辑仍可复用。详见[二次开发 UI 强制重新设计规则](docs/development/ai-guide.zh_CN.md#二次开发-ui-强制重新设计)。
- LVGL 非线程安全。LVGL 任务之外访问 LVGL 对象时必须持有 `bsp_lvgl_lock()`。
- 添加中文 UI 前必须遵循[字体检查清单](docs/development/engineering/coding-conventions.zh_CN.md#中文字体与缺字排查)。默认 Montserrat 字体不含中文字形；UTF-8 正确、编译成功均不代表能够显示中文。必须核对字形覆盖、控件实际字体并完成真机显示验收。
- 按键回调不得阻塞。音频、存储、网络等慢操作必须放入工作任务。
- 删除 screen 前，必须停止所有可能访问其 UI 的任务、定时器、回调和事件处理器。该要求同样适用于 fork 玩法、设备端工具和基线 demo。
- 可测试的状态机、协议、计时和布局计算应与 ESP-IDF/LVGL 解耦，并由 `tests/`（上游）或 `jerryicat/tests/`（fork）中的 host tests 覆盖。
- 禁止提交凭证、设备二维码秘密、私钥、个人数据或未脱敏日志。
- 所有维护中的 Markdown 默认 `.md` 路径必须为英文，简体中文使用配对的 `.zh_CN.md` 文件。两种语言必须保持一致并保留互相切换链接。

## Fork 工作区：`jerryicat/`

- 单一固件。启动器、玩法和设备端工具编译进同一个应用，fork 自研启动器是默认界面。未经用户明确决定，不得为每个玩法建立独立 ESP-IDF 工程，也不得增加第二个 `app_main`。
- `jerryicat/` 是一个 ESP-IDF 组件。`include/` 放公开 API，`src/` 放启动器与共享 UI 套件，`plays/` 放玩法，`apps/` 放设备端工具类应用，`tools/` 放开发侧脚本，`tests/` 放 host tests，`assets/` 放 fork 二进制素材，`docs/` 放 fork 文档。
- 启动器、玩法和设备端工具对自己的页面、布局、导航和按键交互负责。不得以基线 demo 菜单、`demo_*.c` 页面或 `ui_pixel` 外壳为起点。
- 第一个玩法落地时，在仓库根 `CMakeLists.txt` 中通过 `EXTRA_COMPONENT_DIRS` 注册 `jerryicat/` 组件，在 `main/CMakeLists.txt` 的 `REQUIRES` 中加入 `jerryicat`，并让 `main.c` 通过一个小的 `jerryicat_*` API 把启动和按键事件交给启动器。接缝保持最小，并在交付说明中列出。不得把 BSP 或基线 demo 搬进 `jerryicat/`。
- 详细工作区模型、应用生命周期契约和目录地图见 [`jerryicat/AGENTS.zh_CN.md`](jerryicat/AGENTS.zh_CN.md)。

## 按任务加载上下文

| 任务 | 修改前读取 |
| --- | --- |
| 任意代码修改 | `docs/development/ai-guide.zh_CN.md`、相关头文件和相邻实现 |
| fork 工作区概览 | `jerryicat/AGENTS.zh_CN.md`、`jerryicat/README.zh_CN.md` |
| fork 玩法 | `jerryicat/AGENTS.zh_CN.md`、`jerryicat/plays/AGENTS.zh_CN.md`、玩法目录的 `AGENTS.zh_CN.md`、`jerryicat/include/AGENTS.zh_CN.md` |
| fork 设备端工具 | `jerryicat/AGENTS.zh_CN.md`、`jerryicat/apps/AGENTS.zh_CN.md`、应用目录的 `AGENTS.zh_CN.md`、`jerryicat/include/AGENTS.zh_CN.md` |
| 启动器、注册表或共享 UI 套件 | `jerryicat/AGENTS.zh_CN.md`、`jerryicat/src/AGENTS.zh_CN.md`、`jerryicat/include/AGENTS.zh_CN.md` |
| fork 开发侧工具 | `jerryicat/AGENTS.zh_CN.md`、`jerryicat/tools/AGENTS.zh_CN.md` |
| fork host tests | `jerryicat/AGENTS.zh_CN.md`、`jerryicat/tests/AGENTS.zh_CN.md` |
| fork 素材或文档 | `jerryicat/AGENTS.zh_CN.md`、`jerryicat/assets/AGENTS.zh_CN.md` 或 `jerryicat/docs/AGENTS.zh_CN.md` |
| 启动器接入固件 | `jerryicat/AGENTS.zh_CN.md`、`main/main.c`、`CMakeLists.txt`、`main/CMakeLists.txt` |
| 应用工作流或核心技能配置 | `skills/README.zh_CN.md`；确认五个必需技能可用，再使用匹配当前任务的技能 |
| 环境引导或缺少工具链 | `docs/development/engineering/environment-setup.zh_CN.md` |
| BSP、引脚、总线、显示、音频、电池 | `docs/hardware-design/AI_HARDWARE_DEVELOPMENT_GUIDE.zh_CN.md`、`components/bsp/include/bsp_pins.h` |
| 维护上游 demo 或菜单 | `main/demo.h`、`main/main.c`、最近的 `main/demo_*.c` 实现 |
| 中文 UI 或字体 | `docs/development/engineering/lvgl-chinese-fonts.zh_CN.md`、应用字体素材、配置与控件样式 |
| Wi-Fi 联网或蓝牙配网 | `docs/development/engineering/wifi-provisioning.zh_CN.md`、其中引用的 `demo/blufi-provisioning` 实现 |
| 构建、测试、依赖、分区 | `docs/development/engineering/build-and-test.zh_CN.md`、`docs/development/engineering/firmware-layout.zh_CN.md`、`sdkconfig.defaults`、`partitions.csv` |
| CI 或发布 | `docs/development/ci/CI-*.zh_CN.md` 中的对应文件与 `.github/workflows/` |
| 项目开发完成 | `docs/development/release/project-completion.zh_CN.md`（再进入 `issue-suggestions` 或 `experience-pr` skill） |
| 文档 | `docs/contribution/doc-conventions.zh_CN.md`、`docs/README.zh_CN.md` |
| Commit 或 PR | `docs/contribution/commit-and-pr.zh_CN.md` |

上游产品概览与文档索引见 `docs/README.zh_CN.md`。详细的 AI 开发工作流（上下文建立、事实来源优先级、应用/BSP 边界、运行时规则、素材放置、交付格式）见 `docs/development/ai-guide.zh_CN.md`。本 fork 的仓库模型见 `docs/fork-guide.zh_CN.md`。

## 必须执行的验证与交付格式

迭代时运行最小相关检查，交付前运行完整门禁：

```bash
./tools/validate.sh --static    # 仓库检查 + host tests
./tools/validate.sh --firmware  # ESP-IDF 构建 + 合并镜像验证
./tools/validate.sh             # 完整门禁
```

完整门禁要求已激活 ESP-IDF 5.5.3 环境。上游门禁不会自动发现 fork 测试：还必须运行 `jerryicat/tests/` 下的所有 host tests，并在第一个 fork 测试落地时加入 `jerryicat/tools/validate.sh`，让一条命令覆盖 fork 检查。不得把编译成功描述成硬件验证成功。最终交付必须分别报告：

```text
Build: PASS / FAIL / NOT RUN
Host tests: PASS / FAIL / NOT RUN
Device tests: PASS / FAIL / NOT RUN
Unverified: 仍需板卡、仪器或用户确认的事项
```

每次完整实现用户提出的固件需求后，必须主动询问是否将固件刷写到设备中进行
测试，不能只等到发布时才询问。未检测到设备时，提示用户将设备开机，再用
支持数据传输的数据线连接电脑 USB 接口。遵循
[真机测试交接流程](docs/development/ai-guide.zh_CN.md#主动询问真机测试)，
烧录前须取得用户同意；检测到设备本身不代表获得烧录授权。

上游普通功能、应用和文档改动不得修改 `docs/CHANGELOG.md` 或 `docs/CHANGELOG.zh_CN.md`；用户可见行为、兼容性和发布流程影响改为写入 PR 正文及对应权威文档。本 fork 的产品文档放在 `jerryicat/docs/`；只有在明确的发布准备中才同时修改两份变更日志。

社区规范见 `.github/CONTRIBUTING.zh_CN.md`、`.github/CODE_OF_CONDUCT.zh_CN.md`、`.github/SECURITY.zh_CN.md` 与 `.github/SUPPORT.zh_CN.md`。
