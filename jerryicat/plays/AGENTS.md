<p align="right">
  <a href="AGENTS.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# plays/ Rules

Each play is a user-facing gameplay or experience app. One folder per play, one
folder name per app id.

## Folder contract

- Folder name and app id are the same unique lowercase-kebab string, for example
  `snake-run`.
- Every play folder contains its own `AGENTS.md` and `AGENTS.zh_CN.md`. That file
  describes the play's pitch, screens and flow, controls, state and persistence,
  background tasks, teardown, failure states, test plan, and device checks.
- Keep the implementation inside the play folder. Shared code moves to
  `../include/` and `../src/` only when a second app needs it.

## Interface and registration

- Implement the shared on-device app interface from `../include/` and register the
  app in the launcher table in `../src/`. An unregistered play is dead code.
- `id` equals the folder name; the launcher title is short enough for the screen
  and may be localized. Category is `play`.
- `enter` builds the play's screens and resets state. `exit` deletes them after
  work has stopped. Optional `start`/`stop` bracket workers and I/O.
- The play decides its own navigation and long-press behavior. It does not inherit
  the baseline demo menu's click/long-press contract.

## UI and hardware rules

- Design the play's own screens, layout, and visual language. The upstream demo
  menu, `demo_*.c` screens, and `ui_pixel` shell are reference only.
- Portrait 240x320 display, three buttons (`UP`, `DOWN`, `OK`). Keep touch targets
  and labels readable, show the battery level in the top-right corner by default,
  and handle its `-1` unavailable value gracefully.
- Hold `bsp_lvgl_lock()` outside the LVGL task. Key callbacks only enqueue or
  dispatch; audio, storage, and networking run in workers.
- Stop every task, timer, callback, and event handler that can touch the play's UI
  before `exit`. Release audio, network, and storage resources.
- Dim or switch off the backlight when the play idles, and return to light or deep
  sleep when its flow allows it.
- Chinese labels require the font checklist from the root `AGENTS.md`; a UTF-8
  string does not prove glyph coverage.

## State and persistence

- Keep play state inside the play. Do not expose mutable globals to other apps.
- Persist with NVS under the play's own key namespace, document the key names and
  version, and define how stale or corrupt data is reset.
- No credentials, device secrets, or personal data in NVS or logs.

## Tests and documentation

- Put pure logic (scoring, board state, timers, layout math) in plain C modules and
  test them under `../tests/` without ESP-IDF or LVGL.
- Add an entry to the launcher table, the sources to `jerryicat/CMakeLists.txt`,
  and a short manual device-test checklist to the play's `AGENTS.md`.
- Run the upstream gate plus the fork tests, offer on-device testing, and commit
  on `main` per the root maintenance rule.

## Naming

- C files `play_<name>_<topic>.c`, public symbols `jerryicat_play_<name>_*`,
  file-local state `s_`.
- Keep display strings in a small table in the play folder so localization and
  font coverage are reviewable at a glance.
