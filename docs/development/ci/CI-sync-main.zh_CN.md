<p align="right">
  <strong>简体中文</strong> · <a href="CI-sync-main.md">English</a>
</p>

# 上游同步（CI / Upstream Sync）

本仓库提供一套基于 GitHub Actions 的上游同步流水线，用于在用户要求时把上游 `FoloToy/ai-passport` 的 `main` 分支按需同步到本 fork 的 `main` 分支。

本文件与 `.github/workflows/sync-main.yml` 一同维护，工作流行为变化时必须同步更新。

## 触发条件

- **workflow_dispatch**：只能在 GitHub Actions 页面手动触发，且仅在用户要求同步上游时运行。

> 本 fork 直接在 `main` 上开发，并改写了 fork 自有文档，因此自动同步（schedule）已关闭。该工作流仅在仓库为 **fork** 时生效（`if: github.event.repository.fork`）；非 fork 仓库不运行。

## 流水线做了什么

1. **Checkout 目标仓库**：`actions/checkout` 显式设置 `ref: main`，即使手动触发选择其他分支，也检出当前 fork 的 `main`，并关闭 Git 凭证持久化。同步 Action 在 fetch 前需要本地 `main`；否则 `git checkout main` 可能把仓库内的 `main/` 目录作为路径处理，并未切换分支。
2. **同步上游**：使用固定到完整 commit SHA 的 `aormsby/fork-sync-with-upstream-action`（对应 v3.4.3），把 `FoloToy/ai-passport` 的 `main` 同步到本 fork 的 `main`。`target_repo_token` 使用自动生成且仅具 `contents: write` 权限的 `GITHUB_TOKEN`，无需手动配置。
3. **失败检查**：同步失败时输出提示——上游 workflow 文件变更可能导致 GitHub 暂停自动同步，需手动 Sync Fork 一次。

## 注意事项

- 本 fork 直接开发 `main`，不要求 `main` 与上游逐字同步，因此自动同步已关闭；需要同步时，先确认用户意图再手动触发。
- 同步可能修改 `AGENTS.md`、`docs/` 等 fork 自有文档；冲突以本 fork 规则为准解决，不得为采用上游版本整体丢弃 `jerryiscat/` 工作。
- 若同步失败，查看 Actions 日志确认是否为上游 workflow 文件变更所致；必要时按提示手动在 GitHub 页面 Sync Fork。
- 升级 Action 时必须从官方仓库核对目标版本对应的完整 commit SHA，并同步更新 workflow 行尾的版本注释。

## 相关文件

- `.github/workflows/sync-main.yml`：本流水线定义。
