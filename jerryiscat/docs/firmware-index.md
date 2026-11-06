<p align="right">
  <a href="firmware-index.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# Firmware index

This page indexes the firmware images this fork has built or evaluated for the
AI Passport board. It records identity, provenance, and tradeoffs; it is not a
build recipe. The fork's own image is built from `jerryiscat/`. Community images
are separate projects with their own repositories and licenses; indexing one
here does not make it part of this fork's build. The page records provenance
and does not vendor community binaries; rebuild from the pinned commit if a
release disappears.

## Images

| Image | Origin | Identity | Flash | State |
| --- | --- | --- | --- | --- |
| jerryiscat | this fork | commit `422c44d`; merged image 1,572,528 bytes; SHA-256 `a578502a…94bb58` | full image at `0x0` | built 2026-11-06; not on the device after the community image was flashed |
| DSH Passport | `slinxiaosun-blip/ai-passport-dsh` | `v1.3.1-dsh-passport`, commit `57e4aed`; 1,818,528 bytes; SHA-256 `023cca4f…9b26` | full image at `0x0` | flashed 2026-11-07; BLE and desktop connection verified; voice pending |

## jerryiscat

The fork's single firmware: the launcher, Wi-Fi Scan, BLE Beacon, Mic Level, and
Speaker Tone compile into one ESP-IDF application. It uses the fork's own
interface on top of the upstream BSP.

- Build: `idf.py build` with ESP-IDF 5.5.3; merged artifact
  `build/FoloToy-AI-Passport-full.bin`.
- Size: app 1,506,992 bytes; merged 1,572,528 bytes; the factory partition has
  82% free. The built artifact has SHA-256
  `a578502a8552c6a7e5ef256770098f0dfd253e5c49f5ca8a735d2e7e4394bb58`.
- Flash: full image at `0x0`; this replaces the whole application and may reset
  NVS.
- State: build and host checks passed on 2026-11-06. The device currently runs
  DSH Passport, so this image is not on the device.

## DSH Passport

A community firmware that turns the board into a BLE terminal for a desktop DSH
session. It has its own device firmware and companion DSH plugin; it is not part
of this fork's single-firmware build.

- Purpose: aggregated task state, completion banner and sound, tool approvals,
  balance, voice input, and settings/pairing.
- Link: BLE GATT with custom frames and JSON payloads, protocol v1. No Wi-Fi.
  The BLE name is `FoloPassport-DSH`. Pairing uses a six-digit code that changes
  on every boot.
- Host: DSH desktop 0.2.1-rc.5; companion plugin
  `dsh-ai-passport-plugin` 1.3.1 (commit `3b2daee`). The plugin supports macOS
  Apple Silicon and Windows 10+.
- Voice: the device records, but transcription needs the local SenseVoice
  provider (about 240 MB model, disabled by default). Without it the host
  answers that speech is not ready.
- UI: the current firmware is a simplified status card. It shows state, elapsed
  time, and the current tool; it has no task list. Tasks and text input stay on
  the computer.
- Approval: high-risk tools start on "deny"; the countdown rejects on timeout.
- Constraints: flashing from `0x0` erases NVS, so the pairing code and trust
  state are lost. Installing it replaces jerryiscat until jerryiscat is flashed
  again.
- License: firmware MIT, based on `FoloToy/ai-passport` (MIT); companion plugin
  MIT.

## Switching images

Both images are complete merged images written at `0x0`:

```bash
esptool.py --chip esp32c3 -p /dev/cu.usbmodemXXXX -b 460800 \
  write_flash --flash_mode dio --flash_size 8MB --flash_freq 80m \
  0x0 <image.bin>
```

Flashing a full image erases NVS. Pairing state and stored settings are not
preserved. To return to jerryiscat, flash
`build/FoloToy-AI-Passport-full.bin` built from this repository.

## Verification record

- 2026-11-06: jerryiscat build PASS; app 1,506,992 bytes, merged 1,572,528 bytes.
- 2026-11-07: DSH Passport v1.3.1 flashed at `0x0`; esptool reported
  `Hash of data verified`; the BLE advertisement `FoloPassport-DSH` was observed
  at -41 dBm; the desktop plugin connected (user-confirmed). Voice transcription
  was not yet verified.
- Inference, not observation: the 82% free figure comes from the partition table
  and the merged image size, not from a device-side check.

## Provenance

- Firmware: <https://github.com/slinxiaosun-blip/ai-passport-dsh>, tag
  `v1.3.1-dsh-passport`, commit
  `57e4aed4d50934d423c65b99f20017252724ca17`.
- Plugin: <https://github.com/slinxiaosun-blip/dsh-ai-passport-plugin>, version
  1.3.1, commit `3b2daee4e213c60be1b1460a75ec34778607bd2c`.
- Asset: `FoloToy-AI-Passport-full.bin`, 1,818,528 bytes, SHA-256
  `023cca4fbe9aebd38e9ee775f1479bc97720d9947e8705ecb9accbe138029b26`.
