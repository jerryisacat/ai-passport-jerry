# speaker-tone

A speaker sanity check: plays a 500 Hz square wave for two seconds at 40% volume,
then shows `Done`.

## Screen and flow

- Title `SPEAKER TONE`, status `Playing 500 Hz for 2s...`, battery in the top-right.
- The status changes to `Done` when the tone finishes.
- `OK` long press returns to the launcher (reserved by the launcher).

## Lifecycle and resources

- `enter` builds the screen.
- `start` initializes the BSP audio path, wakes the codec, sets 16 kHz/16-bit/mono,
  sets volume 40, and starts a worker task.
- The worker fills 160-sample chunks (320 bytes) with a square wave, writes them
  through `bsp_audio_write()`, and stops after two seconds or on the stop flag.
- `stop` sets the stop flag, waits for the worker, then suspends the codec.
- `exit` deletes the screen after the worker is gone.

## Memory and safety

- One 320-byte stack buffer; no audio file, no NVS, no dynamic allocation.
- The tone is short and quiet; the external PA is always on, so residual standby
  current cannot be switched off in software.

## Checks

- A clear 500 Hz tone is audible for about two seconds.
- Leaving the tool stops the worker and suspends the codec.
- No watchdog reset or pop loud enough to clip.
- Run `../../tools/validate.sh`; speaker behavior needs a device check.
