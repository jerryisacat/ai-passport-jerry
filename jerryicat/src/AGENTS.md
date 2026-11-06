<p align="right">
  <a href="AGENTS.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# src/ Rules

This folder holds the launcher shell, the app registry, and the shared UI kit.
It is the only place that switches between plays and on-device tools.

## Responsibilities

- Launcher UI: the app list or grid, selection, focus, scrolling, transitions,
  and any grouped play/tool presentation.
- App lifecycle orchestration: requesting `stop`, joining or waiting for worker
  shutdown, then calling `exit` before deleting screens.
- Registry: one explicit table of app descriptors. The table order is the
  launcher order; add entries there, never through linker tricks or auto-discovery.
- Shared UI kit: common widgets, battery indicator, headers, empty/error states,
  and time or layout helpers used by more than one app.

Play-specific rules, scoring, screens, and copy do not belong here.

## Runtime rules

- All LVGL work runs in the LVGL task or while holding `bsp_lvgl_lock()`. Never
  hold the lock across blocking work, audio, network calls, or waits for a task.
- The firmware input task calls `jerryicat_handle_key()`; the BSP button callback only enqueues. The launcher dispatches without blocking and reserves long-press `OK` as the return gesture, so that event never reaches the app.
- When leaving an app, stop its work first, then delete its screens, then reset launcher focus and state. A failed app start returns to the launcher with a readable error instead of leaving a half-built screen.
- No app-specific globals. Shared state lives in module-local `s_` state with a
  documented owner and reset path.
- Keep static RAM small: no per-app full-screen buffers, and no large registry
  strings. Content lives in the app's own folder.

## Code shape

- Files `jerryicat_<topic>.c`; internal symbols `static`; file-local state `s_`.
- Extract navigation, selection, and timing logic into plain C modules that the
  host tests can compile without LVGL. UI drawing stays thin.
- Comments explain ownership, call order, and failure behavior, not line-by-line
  mechanics.

## Validation

Add or update tests under `../tests/` for any extracted logic. Render changes need
a device check; a host build does not prove the screen.
