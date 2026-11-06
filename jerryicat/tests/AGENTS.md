<p align="right">
  <a href="AGENTS.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# tests/ Rules

This folder holds host tests for fork logic. They run on the development machine,
never on the device, and never require ESP-IDF or LVGL.

## Scope

- Test pure logic: launcher navigation and selection, app state machines, scoring,
  timers with an injected clock, layout calculations, parsing, and NVS value
  encoding/decoding helpers.
- Do not test LVGL rendering, FreeRTOS scheduling, hardware drivers, or real audio
  here. Those need the firmware build and a device check.

## Conventions

- C tests: `test_<module>.c`, C11, compiled with
  `-std=c11 -Wall -Wextra -Werror`. Link only the module under test and small
  stubs from `tests/stubs/` when unavoidable.
- Python tests: `test_<module>.py`, standard library only, runnable directly.
- One behavior per test with a plain assertion and a message that names the
  expected result. Test failure paths, boundary values, and reset behavior, not
  private implementation details.
- No sleeps, no network, no hardware, no randomness without a fixed seed.
- Keep tests fast enough to run on every change.

## Wiring and validation

- Add or update tests in the same round as the code they cover.
- Wire every test into `../tools/validate.sh` when that script exists; until then,
  run each test file explicitly.
- Report host-test results separately from the firmware build and from device
  results. A passing host test does not prove the screen, audio, or power
  behavior.

## Naming

- Group by the module under test, not by the app that first needed it.
- Shared stubs live in `tests/stubs/` and must be minimal, documented, and free of
  product logic.
