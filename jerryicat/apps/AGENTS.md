<p align="right">
  <a href="AGENTS.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# apps/ Rules

Each folder here is an on-device utility app — a timer, checklist, clock, guide,
or similar tool the user opens from the launcher. It is part of the firmware, not
a host script.

## Folder contract

- Folder name and app id are the same unique lowercase-kebab string, for example
  `pomodoro`.
- Every app folder contains its own `AGENTS.md` and `AGENTS.zh_CN.md` describing
  purpose, screens and flow, controls, persisted data, tasks, teardown, failure
  states, tests, and device checks.
- Host scripts do not live here; they belong in `../tools/`.

## Interface and registration

- Implement the same on-device app interface as plays, with category `tool`.
- Register in the launcher table in `../src/`; `id` equals the folder name.
- The launcher reserves long-press `OK` as the return gesture; the app owns its
  short-press behavior and any additional gestures. Do not inherit the baseline
  demo contract.
- `enter` builds and resets; `exit` deletes screens after work stops; optional
  `start`/`stop` bracket workers and I/O.

## Product rules

- Utility apps must be genuinely useful offline. Do not add networking, accounts,
  or cloud dependencies unless the user asks for them.
- Persist user data with NVS under the app's own namespace. Document keys,
  formats, defaults, and reset behavior. Never store credentials or personal data.
- Treat time carefully: the ESP32-C3 has no battery-backed wall clock, so local
  time is lost across power cycles unless the app syncs or asks the user to set
  it. Do not promise persistence the hardware cannot provide.
- Show the battery level in the top-right corner by default and handle `-1`.
- Keep screens legible and calm: readable type, clear empty states, no decorative
  clutter that competes with the tool's content.
- Redesign the UI for the app's purpose; the upstream demo menu and `ui_pixel`
  shell are reference only.

## Runtime rules

- Hold `bsp_lvgl_lock()` outside the LVGL task; keep key callbacks non-blocking.
- Stop tasks, timers, callbacks, and event handlers before deleting screens or
  leaving the app.
- Dim or switch off the backlight when idle, and return to sleep when the app
  allows it.
- Budget RAM: no PSRAM, no large static buffers. Load fonts deliberately and
  follow the Chinese font checklist before shipping Chinese text.

## Tests and documentation

- Test pure logic (time math, list state, layout calculations) under `../tests/`
  without ESP-IDF or LVGL.
- Add sources to `jerryicat/CMakeLists.txt`, update the launcher and the folder map
  in `../README.md`, and keep the app's `AGENTS.md` test checklist current.
- Run the gates, offer on-device testing, and commit on `main` per the root rule.
