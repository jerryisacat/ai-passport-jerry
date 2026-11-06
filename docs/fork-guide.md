<p align="right">
  <a href="fork-guide.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# Fork Workflow

This repository is a downstream copy of FoloToy/ai-passport. Upstream provides the
hardware contract, BSP, baseline demo firmware, engineering references, AI skills,
and validation tooling. That material is the foundation, not this fork's product.

Fork-owned work — the launcher, plays, on-device tools, host-side tools, assets,
and documentation — lives under `jerryicat/`. The repository-wide maintenance rules
are in [`../AGENTS.md`](../AGENTS.md); the workspace model is in
[`../jerryicat/AGENTS.md`](../jerryicat/AGENTS.md), with one `AGENTS.md` per folder.

## Repository roles

```text
components/bsp/        upstream board APIs and hardware implementation
main/                  firmware entry and upstream hardware-test demo
jerryicat/             fork launcher, plays, on-device tools, host tools, assets, docs
assets/                upstream reusable fonts, images, music, and sound effects
skills/                upstream AI-agent skills
tests/                 upstream host tests
docs/                  upstream product, development, design, and reference documents
sdkconfig.defaults     reproducible ESP32-C3 defaults
```

## Maintenance model

- `main` is the working branch. Develop and document directly on it; do not create
  branches, worktrees, or pull requests unless the user explicitly asks.
- After each completed round, commit the intended changes and push to
  `origin/main`. The standing rule in `AGENTS.md` authorizes these pushes; do not
  wait for a separate request each round.
- Keep upstream files usable as the baseline. Prefer adding under `jerryicat/`
  over editing upstream code. The only expected upstream edits are the minimal
  launcher integration in `main/main.c`, `CMakeLists.txt`, and
  `main/CMakeLists.txt`, plus genuine upstream fixes.
- Do not delete or repurpose upstream demo pages, documentation, or skills without
  an explicit request. The baseline demo remains a hardware reference.
- Automatic upstream synchronization is disabled. Fetch or merge from `upstream`
  only when the user asks, and resolve conflicts in favor of this file and the
  `jerryicat/` rules. Never drop fork work to take an upstream version wholesale.
- Open an upstream pull request only when the user asks for it, and only for a
  general fix that benefits every AI Passport user. Fork-private product content
  stays here.

## Documentation split

- Upstream hardware, BSP, engineering, contribution, and reference documents stay
  in `docs/` and apply to work on the baseline.
- Fork architecture, play specifications, tool usage, and product decisions live
  in `jerryicat/docs/` or beside the app or tool. They are not proposed upstream.
- A general documentation improvement that helps any AI Passport user may go
  upstream, but it must not carry fork-specific product details.
- The root `README.md` path remains optional and fork-owned. Add one only when the
  user asks for a human-facing landing page.

## Language

All fork documentation follows the repository rule: English at the default `.md`
path and Simplified Chinese at the paired `.zh_CN.md` path, with reciprocal
language links.
