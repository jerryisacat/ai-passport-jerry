# wifi-scan

A 2.4 GHz scan tool. It lists nearby SSIDs, RSSI, and channel. It never connects,
never stores credentials, and never writes Wi-Fi state to NVS.

## Screen and flow

- Title `WI-FI SCAN`, battery in the top-right, status line, up to five AP rows.
- `OK` long press returns to the launcher (reserved by the launcher).
- While scanning, the status reads `Scanning 2.4 GHz...`; an 8-second timeout
  switches to a failure message.

## Lifecycle and resources

- `enter` builds the screen and a 100 ms LVGL timer.
- `start` prepares NVS/netif/event loop, creates a checked STA netif, initializes
  Wi-Fi with `WIFI_STORAGE_RAM`, starts a non-blocking scan, and returns.
- `WIFI_EVENT_SCAN_DONE` advances the state; the LVGL timer reads the five stored
  records and renders them.
- `stop` stops the scan/Wi-Fi stack and destroys the STA netif. NVS, netif, and the
  default event loop stay initialized for later tools.
- No worker task is needed; the scan result handling runs in the LVGL timer.

## Memory and safety

- Five `wifi_ap_record_t` records and one 320-byte text buffer, both static.
- No connection, no DHCP, no credentials, no NVS writes.
- The checked `esp_netif_new` path avoids the aborting convenience helper.

## Checks

- The status changes from scanning to a result or an error; no watchdog reset.
- A phone hotspot in range appears with a plausible RSSI.
- Leaving the tool releases the Wi-Fi stack; re-entering scans again.
- Run `../../tools/validate.sh`; firmware and device checks are separate.
