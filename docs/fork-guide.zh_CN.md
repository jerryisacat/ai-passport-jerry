<p align="right">
  <strong>简体中文</strong> · <a href="fork-guide.md">English</a>
</p>

# Fork 工作流

本仓库是 FoloToy/ai-passport 的下游副本。上游提供硬件契约、BSP、基线演示固件、工程参考、AI 技能和验证工具；这些是开发基础，而不是本 fork 的产品。

本 fork 自研的一切——启动器、玩法、设备端工具、开发侧工具、素材和文档——都放在 `jerryiscat/` 下。仓库级维护规则见 [`../AGENTS.md`](../AGENTS.md)；工作区模型见 [`../jerryiscat/AGENTS.md`](../jerryiscat/AGENTS.md)，每个目录另有自己的 `AGENTS.md`。

## 目录职责

```text
components/bsp/        上游板级 API 与硬件实现
main/                  固件入口与上游硬件测试 demo
jerryiscat/             fork 启动器、玩法、设备端工具、开发侧工具、素材与文档
assets/                上游可复用的字体、图片、音乐与音效
skills/                上游 AI agent 技能
tests/                 上游 host tests
docs/                  上游产品、开发、设计、参考与社区文档
sdkconfig.defaults     可复现的 ESP32-C3 默认配置
```

## 维护模型

- `main` 是工作分支。开发和文档都直接在其上进行；除非用户明确要求，不创建分支、worktree 或 Pull Request。
- 每轮工作完成后，提交本轮改动并推送到 `origin/main`。`AGENTS.md` 中的长期规则即为这些推送的授权，不需要每轮另行等待用户确认。
- 保持上游文件可作为基线使用。优先在 `jerryiscat/` 下新增，而不是修改上游代码。预期中的上游改动只有 `main/main.c`、`CMakeLists.txt` 和 `main/CMakeLists.txt` 中的最小启动器接线，以及真实的上游缺陷修复。
- 未经明确要求，不得删除或挪用上游 demo 页面、文档或技能。基线 demo 仍是硬件参考。
- 自动上游同步已关闭。只有用户要求时才从 `upstream` fetch 或 merge，冲突以本文和 `jerryiscat/` 规则为准解决。不得为了采用上游版本而整体丢弃 fork 工作。
- 只有用户要求时才向上游开 Pull Request，而且只提交对所有 AI Passport 用户都有益的通用修复。fork 私有产品内容留在这里。

## 文档与经验的分区

- 上游硬件、BSP、工程、协作与参考文档留在 `docs/`，适用于基线开发。
- fork 架构、玩法规格、工具用法与产品决策放在 `jerryiscat/docs/` 或对应应用/工具目录，不提交回上游。
- 对任何 AI Passport 用户都有益的通用文档改进可以进入上游，但不得携带 fork 私有产品细节。
- 根 `README.md` 仍为可选、由 fork 自行决定。只有用户要求时，才添加面向人的落地页。

## 语言

fork 的所有文档遵循仓库规则：默认 `.md` 使用英文，`name.zh_CN.md` 使用简体中文，并保留互相切换链接。
