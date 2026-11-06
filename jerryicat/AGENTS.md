<p align="right">
  <a href="AGENTS.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# jerryicat Workspace Rules

This folder is the fork's own product layer. The root [`AGENTS.md`](../AGENTS.md) governs the whole repository; this file governs `jerryicat/`, and each subfolder has its own `AGENTS.md` with folder-specific detail. The nearest file wins for its subtree and may override this file; none of them override the root safety baseline.

The human-facing overview is [`README.md`](README.md).

## What belongs here

- The launcher shell, shared UI kit, plays, on-device tools, host-side tools, fork tests, fork assets, and fork documentation.
- Code that builds a product experience on top of the upstream BSP. The BSP itself, the upstream hardware-test demo, and the upstream validation gate stay where they are.

## Build model

- One firmware. The launcher, plays, and on-device tools compile into the single application; the launcher is the default UI.
- `jerryicat/` is one ESP-IDF component named `jerryicat`. There is exactly one `app_main`, in `main/main.c`; this folder never defines a second one.
- Wiring: `jerryicat/CMakeLists.txt` registers the component, the root `CMakeLists.txt` adds it through `EXTRA_COMPONENT_DIRS`, `main/CMakeLists.txt` lists it in `REQUIRES`, `main.c` calls `jerryicat_launcher_start()`, and the firmware input task forwards key events to `jerryicat_handle_key()`. Keep that seam minimal.
- Do not create a separate ESP-IDF project, per-play `sdkconfig`, or duplicated partition table unless the user explicitly changes the one-firmware decision.

## Architecture

- `include/` — public headers consumed by `main/` and sibling folders: launcher entry, on-device app interface, shared UI-kit declarations.
- `src/` — launcher shell, app registry, navigation, shared UI and runtime helpers.
- `plays/` — gameplay and experience apps, one folder per play.
- `apps/` — on-device utility apps, one folder per tool.
- `tools/` — host-side development scripts; never firmware sources.
- `tests/` — host tests for fork logic; no ESP-IDF or LVGL dependency.
- `assets/` — fork-only fonts, images, and audio with provenance records.
- `docs/` — fork architecture notes, play specifications, and product decisions.

## On-device app contract

Plays and on-device tools share one lifecycle interface declared in `include/`; only their launcher category differs.

- Each app is described by an id (unique lowercase-kebab, identical to its folder name), a launcher title, a category (`play` or `tool`), and lifecycle callbacks: `enter`, `exit`, key handling, and optional `start`/`stop` for asynchronous work.
- `enter` builds the app's screens and resets its state; `exit` deletes those screens after all work has stopped. `start`/`stop` bracket worker tasks, audio, networking, and persistence that must not run while the app is hidden.
- Key callbacks dispatch lightweight events only. The launcher reserves a long press of `OK` as the return gesture; apps own short-press behavior and any additional gestures, not a copy of the demo menu.
- Register the app in the launcher table. An unregistered app is dead code; the build must not silently include it.
- Every app owns its visual language. The baseline demo menu, `demo_*.c` screens, and the `ui_pixel` shell are reference material, never a starting point.

## Runtime rules

- Hold `bsp_lvgl_lock()` whenever a non-LVGL task touches LVGL objects.
- Stop every task, timer, callback, and event handler that can reach an app's UI before deleting its screen or leaving the app. Return audio, network, and storage resources to their idle state.
- Show the battery level in the top-right corner by default via `bsp_battery_soc()`; render nothing when it returns `-1`, and keep it clear of the app's own content.
- Dim or switch off the backlight when the screen idles, and return to light or deep sleep where the app's flow allows it.
- Budget internal RAM: no PSRAM, no large static buffers per play. Load fonts and images deliberately and release what the app no longer needs.
- Chinese UI is a font-integration task. Follow the root font checklist before shipping any Chinese label.

## Folder map

| Folder | Purpose | Local rules |
| --- | --- | --- |
| [`include/`](include/AGENTS.md) | Public launcher and app-interface headers | `include/AGENTS.md` |
| [`src/`](src/AGENTS.md) | Launcher shell, registry, shared UI kit | `src/AGENTS.md` |
| [`plays/`](plays/AGENTS.md) | Gameplay and experience apps | `plays/AGENTS.md` |
| [`apps/`](apps/AGENTS.md) | On-device utility apps | `apps/AGENTS.md` |
| [`tools/`](tools/AGENTS.md) | Host-side development scripts | `tools/AGENTS.md` |
| [`tests/`](tests/AGENTS.md) | Host tests for fork logic | `tests/AGENTS.md` |
| [`assets/`](assets/AGENTS.md) | Fork fonts, images, and audio | `assets/AGENTS.md` |
| [`docs/`](docs/AGENTS.md) | Fork design and product documentation | `docs/AGENTS.md` |

## Adding a play or an on-device tool

1. Read the folder rules (`plays/AGENTS.md` or `apps/AGENTS.md`) and the interface headers in `include/`.
2. Create the folder with a lowercase-kebab name and add its own `AGENTS.md` and `AGENTS.zh_CN.md` describing purpose, screens, controls, state, persistence, tasks, teardown, tests, and device checks.
3. Implement the shared lifecycle interface and register the app in the launcher table.
4. Add the sources to `jerryicat/CMakeLists.txt`, add host tests for pure logic, and wire the tests into `jerryicat/tools/validate.sh`.
5. Update [`README.md`](README.md) and any related notes in `docs/`.
6. Run the upstream gate plus the fork tests, offer on-device testing, then commit and push on `main` per the root maintenance rule.

## Status

The launcher and the first play, `+1`, are implemented and wired into the single
firmware. New plays and on-device tools follow the same interface, registry, tests,
and folder `AGENTS.md` pattern.
