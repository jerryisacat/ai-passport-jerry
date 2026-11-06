<p align="right">
  <a href="README.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# jerryicat

This directory is the fork's own product layer on top of the FoloToy AI Passport
baseline. The upstream BSP, baseline demo, engineering references, and validation
gate remain the foundation; everything this fork builds — launcher, plays,
on-device tools, host-side tools, assets, and documentation — lives here.

The agent-facing rules are in [`AGENTS.md`](AGENTS.md). Each subfolder has its
own `AGENTS.md` with its detailed working rules.

## Status

The launcher and the first play, `+1`, are implemented and wired into the single
firmware. New apps follow the same interface, registry, tests, and folder
`AGENTS.md` pattern.

## Apps

| App | Folder | Behavior | Rules |
| --- | --- | --- | --- |
| `+1` | `plays/plus-one/` | Any button press adds one; hold `OK` returns to the launcher | [`AGENTS.md`](plays/plus-one/AGENTS.md) |

## Layout

| Path | Purpose | Rules |
| --- | --- | --- |
| `include/` | Public launcher and on-device app interface headers | [`include/AGENTS.md`](include/AGENTS.md) |
| `src/` | Launcher shell, app registry, shared UI kit | [`src/AGENTS.md`](src/AGENTS.md) |
| `plays/` | Gameplay and experience apps, one folder each | [`plays/AGENTS.md`](plays/AGENTS.md) |
| `apps/` | On-device utility apps, one folder each | [`apps/AGENTS.md`](apps/AGENTS.md) |
| `tools/` | Host-side development scripts | [`tools/AGENTS.md`](tools/AGENTS.md) |
| `tests/` | Host tests for fork logic | [`tests/AGENTS.md`](tests/AGENTS.md) |
| `assets/` | Fork fonts, images, and audio with provenance | [`assets/AGENTS.md`](assets/AGENTS.md) |
| `docs/` | Fork design notes and product documentation | [`docs/AGENTS.md`](docs/AGENTS.md) |

## Product model

- One firmware for the device. The launcher, every play, and every on-device tool
  compile into one application; the launcher is what the user sees first.
- Plays and on-device tools share one lifecycle interface: enter, exit, key
  handling, and optional start/stop for asynchronous work.
- Applications own their screens, controls, and visual design. The upstream demo
  menu and `ui_pixel` shell are reference only.
- Host tools and host tests stay outside the firmware, under `tools/` and `tests/`.

## Working here

1. Read the root [`AGENTS.md`](../AGENTS.md) and this workspace's
   [`AGENTS.md`](AGENTS.md).
2. Read the `AGENTS.md` of the folder you are about to touch.
3. Implement the change with its tests and documentation pair.
4. Run the upstream gate (`./tools/validate.sh`) plus the fork host tests
   (`./jerryicat/tools/validate.sh`).
5. Offer on-device testing for firmware changes, then commit and push on `main`
   as required by the root maintenance rule.

For the repository model, see [`docs/fork-guide.md`](../docs/fork-guide.md).
