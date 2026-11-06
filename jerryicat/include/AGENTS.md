<p align="right">
  <a href="AGENTS.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# include/ Rules

This folder holds the public headers the launcher, plays, on-device tools, and
`main/` use to talk to each other. Everything here is a contract; implementation
lives in `../src/` or in the app folders.

## What goes here

- The launcher entry API used by `main/main.c`.
- The on-device app interface: descriptor, category, and lifecycle callbacks.
- Shared UI-kit and runtime-helper declarations that more than one folder needs.

Nothing else. A header used by one `.c` file belongs next to that file.

## Conventions

- Name files `jerryicat_<topic>.h`; use include guards `JERRYICAT_<TOPIC>_H`.
- Public symbols use the `jerryicat_` prefix; public types end in `_t`.
- Headers are self-contained: include what they use and forward-declare where the
  type is opaque. Do not rely on include order.
- Keep LVGL and ESP-IDF types out of public signatures when a plain C type works.
  An app callback should receive key events, not `lv_obj_t *`.
- No board constants here — pins, addresses, and panel parameters stay in
  `components/bsp/include/bsp_pins.h`.
- No logging, allocation, or static state in headers.

## Documentation requirements

For every declaration, state the purpose, parameters, return values and failure
values, memory ownership, task context, blocking behavior, and whether the caller
must hold `bsp_lvgl_lock()`. For the app interface, also state the call order and
who stops asynchronous work before `exit`.

## Changing the interface

The app interface is the shared contract for `../plays/`, `../apps/`, and the
launcher. A change updates every implementation and the folder AGENTS files that
describe it, in the same round. Do not add a parallel interface for one app.

## Validation

Headers are validated by the firmware build and by the host tests that include
them. A header-only change still needs `./tools/validate.sh --static`.
