# mic-level

A live microphone level meter. It reads the ES8311 capture path at 16 kHz,
16-bit mono, shows a peak bar, and stores nothing.

## Screen and flow

- Title `MIC LEVEL`, a 0–100 bar, `Listening...`, battery in the top-right.
- `OK` long press returns to the launcher (reserved by the launcher).

## Lifecycle and resources

- `enter` builds the bar and labels.
- `start` initializes the BSP audio path, wakes the codec, sets 16 kHz/16-bit/mono,
  mutes the output, and starts a worker task.
- The worker reads 160-sample chunks (320 bytes), computes a smoothed peak, and
  updates the bar at most every three chunks while holding `bsp_lvgl_lock()`.
- `stop` sets the stop flag, waits for the worker, then suspends the codec with
  `bsp_audio_sleep()`.
- `exit` deletes the screen after the worker is gone.

## Memory and safety

- One 320-byte stack buffer; no recording buffer, no NVS, no file writes.
- No audio is stored or transmitted.
- The output volume is set to 0 while metering to avoid feedback.

## Checks

- The bar responds to speech and falls back when quiet.
- No feedback howl; no watchdog reset.
- Leaving the tool suspends the codec and stops the worker.
- Run `../../tools/validate.sh`; microphone behavior needs a device check.
