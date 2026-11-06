<p align="right">
  <a href="AGENTS.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# plus-one

The smallest complete play: one number on screen, and every press of any of the
three buttons adds one. It exists to prove the launcher, input, and lifecycle
path end to end.

## Screens and flow

- Launcher entry: title `+1`, category `PLAY`.
- Counter screen: brand `+1` top-left, battery top-right, a large accent count in
  the middle, `PRESSES THIS SESSION` below it, and the bottom hint
  `PRESS ANY KEY +1 / HOLD OK BACK`.
- Leaving the play and opening it again keeps the in-session count.

## Controls

- Any of `UP`, `DOWN`, or `OK` pressed down: add one. Counting on the press event
  means every physical press counts exactly once, including rapid presses and
  double taps.
- `OK` long press: reserved by the launcher as the return gesture. The press that
  starts the hold already counted, like any other press.
- `CLICK`, `DOUBLE`, `RELEASE`, and long presses of `UP`/`DOWN` do not add again.

## State and persistence

- A file-local `uint32_t` that saturates at `UINT32_MAX` instead of wrapping.
- Session-only. There is no NVS write, so a reboot resets the count; the play
  notes this limitation instead of pretending to persist it.

## Tasks, resources, and teardown

- No worker task, timer, audio, networking, or storage access.
- No idle backlight or sleep policy yet; the screen stays on while the play is
  open. Add a timeout before treating this play as power-final.
- `enter` builds the screen with the LVGL lock already held and loads it.
- `exit` deletes the screen and clears the widget pointers; the count survives.

## Tests

- `../../tests/test_plus_one_logic.c` covers increment and saturation through the
  pure `plus_one_logic.h` helper.
- Run `../../tools/validate.sh` for the fork host tests.

## Device checks

- Every physical press adds exactly one; rapid presses and double taps are not
  lost.
- Long-press `OK` returns to the launcher; the initiating press counts once.
- Reopening `+1` keeps the count; rebooting starts at 0.
- The count is readable at 48 px and the battery area is blank when the gauge is
  unavailable.

## Files

- `plus_one.c` — app descriptor, screen, key handling.
- `plus_one_logic.h` — saturating increment, host-tested, no ESP-IDF or LVGL.
