<p align="right">
  <a href="AGENTS.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# tools/ Rules

This folder holds host-side development scripts: build and flash helpers, asset
converters, release preparation, and checks that support the fork's workflow.
Firmware sources never live here.

## Tool contract

- Prefer Python 3 with the standard library. Use a shell script only when it is
  genuinely simpler, and keep it portable across macOS and Linux.
- Every tool has a `--help` text that states purpose, inputs, outputs, side
  effects, and examples. Exit `0` on success, `1` on a handled failure, and `2`
  on invalid usage.
- Resolve paths from the repository root, not the current directory. Never assume
  the caller's working directory.
- Be deterministic and safe to rerun. A second run with the same inputs produces
  the same result and does not double-apply changes.
- Do not prompt interactively unless the tool documents it; support an explicit
  `--yes` for destructive actions.
- Write build products to `build/` or a documented temporary directory. Never
  commit generated output, firmware images, or logs.

## Safety

- No credentials, tokens, device secrets, private keys, personal data, or raw
  device logs in source, arguments, environment defaults, or output.
- No network access by default. A tool that downloads must say so in `--help`, use
  HTTPS, and pin or verify what it downloads.
- Destructive operations must print the resolved absolute target and require
  confirmation.
- Do not modify upstream `tools/`; call `../../tools/validate.sh` or the specific
  upstream script instead. Do not weaken a check by reimplementing it here.

## Tests and documentation

- Add a host test under `../tests/` for any parsing, planning, or transformation
  logic that is not a thin wrapper around an external command.
- Register every tool in the table in [`../README.md`](../README.md) or the
  tool's own README pair, with a one-line purpose and an example.
- When a tool becomes part of the standard gate, wire it into
  `validate.sh` here and document the new command in the root `AGENTS.md`.

## Naming

- Scripts use lowercase-kebab names, for example `flash-device.py`.
- Keep shared helpers in a small module next to the scripts; do not build a
  framework before a second tool needs it.
