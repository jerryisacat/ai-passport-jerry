# ble-beacon

A non-connectable Bluetooth LE advertising tool. A phone BLE scanner should see
`jerryiscat`. There is no pairing, bonding, GATT service, or Bluetooth Classic.

## Screen and flow

- Title `BLE BEACON`, status `Advertising` with the device name, battery in the
  top-right.
- `OK` long press returns to the launcher (reserved by the launcher).

## Lifecycle and resources

- `start` prepares NVS, initializes NimBLE, registers GAP/GATT, sets the device
  name, and creates the NimBLE host task.
- On sync, the tool ensures an address, infers the address type, and starts
  general non-connectable advertising.
- `stop` stops advertising, stops the NimBLE port, waits for the host task to
  acknowledge, deletes that task, and deinitializes the port.
- The NimBLE stack runs only while this tool is open.

## Memory and safety

- Host task stack uses `NIMBLE_HS_STACK_SIZE`; no dynamic advertising payloads.
- No pairing data, secrets, or credentials; NVS is only initialized, never erased.
- Advertising is non-connectable, so there is no GATT server to maintain.

## Checks

- A phone BLE scanner sees `jerryiscat`.
- Leaving the tool stops advertising; re-entering starts it again.
- No reboot or watchdog reset during start/stop.
- Run `../../tools/validate.sh`; verify the radio on a device separately.
