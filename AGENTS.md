<p align="right">
  <a href="AGENTS.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# Repository Guidelines for AI Agents

This file is the only mandatory entry point for AI-assisted work in this repository. It governs the whole checkout. Read the task-specific documents from the routing table below; do not load every README by default.

## Repository role

This checkout is a downstream copy of [`FoloToy/ai-passport`](https://github.com/FoloToy/ai-passport), tracked as the `upstream` remote. Upstream provides the hardware contract, BSP, baseline demo firmware, engineering references, AI skills, and validation tooling. Treat that material as the foundation and as the source of hardware truth.

Everything this fork develops — the launcher, plays, on-device tools, host-side tools, fork assets, and fork documentation — lives under [`jerryicat/`](jerryicat/AGENTS.md). Do not scatter first-party product code into `main/`, `components/`, `tools/`, or `tests/`.

Every folder under `jerryicat/` has its own `AGENTS.md` describing what belongs there and how to work in it. Read `jerryicat/AGENTS.md` and the `AGENTS.md` of every folder you touch. The nearest file governs its subtree and may add detail to, or override, broader guidance; it never overrides the root safety rules below.

## Repository maintenance

- Work directly on `main` in this checkout. Do not create branches, worktrees, or pull requests unless the user explicitly asks for them.
- After each completed round of development or documentation work, commit the intended changes and push them to `origin/main`. This standing rule is the authorization for those commits and pushes; do not wait for a separate request each round.
- Review the complete diff first and stage only the files that belong to the round. Never sweep unrelated user changes or untracked files into the commit.
- Run the applicable checks before committing. Do not push a known failure; fix it or report the blocker.
- Use the Conventional Commit form from `docs/contribution/commit-and-pr.md`: English imperative subject, for example `docs(agents): record the fork workspace model`.
- Push only to `origin`. Never push to `upstream`, force-push, rewrite published history, or move tags.
- Automatic upstream synchronization is disabled. Fetch or merge from `upstream` only when the user asks; resolve conflicts in favor of this file and the `jerryicat/` rules, and never drop fork work to take an upstream version wholesale.
- Upstream guidance that keeps `main` synchronized or tells developers to work on `feature/*` branches is superseded by this section.

## Required AI skills

The following five skills are required for AI-assisted development in this repository:
`passport-develop`, `passport-setup`, `passport-build`, `passport-device-test`, and
`passport-debug`. Their maintained sources are under `skills/`.

Before starting development, the AI must check that all five are installed and
available in its current environment. If any are missing, the AI must choose an
appropriate installation method for its tool and environment, perform the
installation, and verify availability itself. Do not wait for the user to request
installation or make the user choose the method or run installation commands.
No particular script, installation directory, or AI client is mandatory.

Respect the environment's approval requirements and preserve existing skills
and user configuration. If permissions, conflicts, or platform limitations prevent
installation, explain the blocker and request only the necessary user action;
do not claim installation succeeded. Having all five available does not mean
loading or invoking all five for every task: use only the matching skills.
Flashing and publishing still require separate authorization; Git writes follow
the maintenance rule above.

These skills describe the upstream development workflow. Their setup, build,
device-test, and debug stages still apply here; their application-placement
defaults are overridden by the `jerryicat/` rules.

## Project and safety baseline

- Target: ESP32-C3, 8 MB Flash, no PSRAM, ESP-IDF 5.5.3.
- Keep the repository's default partition table minimal: NVS, PHY data, and
  one factory application spanning the rest of the 8 MB Flash. User firmware
  may deliberately change this layout; validate the resulting table and do not
  turn product-specific partitions into mandatory template contracts.
- Preserve existing user changes. Start with `git status --short --branch`; never overwrite or clean unrelated files.
- Flashing new firmware does not require backing up the firmware already on the device; do not make a Flash readback a prerequisite. This does not guarantee preservation of user data or authorize a full-chip erase. Follow the [flashing and data policy](docs/development/engineering/firmware-layout.md#flashing-and-stored-data).
- Hardware facts follow this priority: product specifications and measured results → `components/bsp/include/bsp_pins.h` → BSP headers and implementation → hardware guide → README/demo code. If a task requires a hardware detail not defined by these sources, ask the user instead of guessing.
- Reusable board logic belongs in `components/bsp`. The launcher, shared UI kit, plays, on-device tools, and their tasks belong under `jerryicat/`. `main/` remains the firmware entry and keeps the upstream demo sources as reference; the boot UI comes from the launcher, and product logic must not grow in `main/`.
- Derivative applications must redesign and implement their own UI. Reusing the current demo test menu, screens, or visual shell is prohibited; renaming, recoloring, or adding a feature to that shell does not count as a redesign. BSP APIs and non-UI logic remain reusable. See the [mandatory UI redesign rule](docs/development/ai-guide.md#mandatory-ui-redesign-for-derivative-applications).
- LVGL is not thread-safe. Code outside the LVGL task must hold `bsp_lvgl_lock()` while accessing LVGL objects.
- Before adding Chinese UI text, follow the [font checklist](docs/development/engineering/coding-conventions.md#chinese-fonts-and-missing-glyphs). The default Montserrat fonts have no Chinese glyphs; UTF-8 and a successful build do not prove display support. Verify glyph coverage, widget font selection, and on-device rendering.
- Button callbacks must stay non-blocking. Audio, storage, networking, and other slow operations belong in worker tasks.
- Before deleting a screen, stop every task, timer, callback, and event handler that can access its UI. This applies to fork plays and on-device tools as well as the baseline demo.
- Keep testable state machines, protocols, timing, and layout calculations independent from ESP-IDF/LVGL and cover them with host tests under `tests/` (upstream) or `jerryicat/tests/` (fork).
- Never commit credentials, device QR secrets, private keys, personal data, or unsanitized logs.
- Every maintained Markdown document uses English at its default `.md` path and Simplified Chinese in a paired `.zh_CN.md` file. Keep both versions aligned and retain reciprocal language links.

## Fork workspace: `jerryicat/`

- One firmware. The launcher, plays, and on-device tools compile into the single application, and the fork launcher is the default user interface. Do not create a separate ESP-IDF project per play or a second `app_main` without an explicit user decision.
- `jerryicat/` is one ESP-IDF component. `include/` holds public APIs, `src/` the launcher and shared UI kit, `plays/` gameplay apps, `apps/` on-device utility apps, `tools/` host-side development scripts, `tests/` host tests, `assets/` fork binary assets, and `docs/` fork documentation.
- The launcher, plays, and on-device tools own every screen, layout, navigation, and button interaction they present. Never start from the baseline demo menu, `demo_*.c` screens, or the `ui_pixel` shell.
- The component is registered through `EXTRA_COMPONENT_DIRS` in the root `CMakeLists.txt`, `main/CMakeLists.txt` lists `jerryicat` in `REQUIRES`, and `main.c` calls `jerryicat_launcher_start()` and forwards key events to `jerryicat_handle_key()`. Keep that seam minimal. Do not move the BSP or the baseline demo into `jerryicat/`.
- The detailed workspace model, app lifecycle contract, and folder map are in [`jerryicat/AGENTS.md`](jerryicat/AGENTS.md).

## Task-specific context routing

| Task | Read before editing |
| --- | --- |
| Any code change | `docs/development/ai-guide.md`, relevant headers and neighboring implementation |
| Fork workspace overview | `jerryicat/AGENTS.md`, `jerryicat/README.md` |
| Fork play | `jerryicat/AGENTS.md`, `jerryicat/plays/AGENTS.md`, the play folder `AGENTS.md`, `jerryicat/include/AGENTS.md` |
| Fork on-device tool | `jerryicat/AGENTS.md`, `jerryicat/apps/AGENTS.md`, the app folder `AGENTS.md`, `jerryicat/include/AGENTS.md` |
| Launcher, registry, or shared UI kit | `jerryicat/AGENTS.md`, `jerryicat/src/AGENTS.md`, `jerryicat/include/AGENTS.md` |
| Fork host tool | `jerryicat/AGENTS.md`, `jerryicat/tools/AGENTS.md` |
| Fork host tests | `jerryicat/AGENTS.md`, `jerryicat/tests/AGENTS.md` |
| Fork assets or documentation | `jerryicat/AGENTS.md`, `jerryicat/assets/AGENTS.md` or `jerryicat/docs/AGENTS.md` |
| Launcher integration into firmware | `jerryicat/AGENTS.md`, `main/main.c`, `CMakeLists.txt`, `main/CMakeLists.txt` |
| Application workflow or core skill setup | `skills/README.md`; ensure the five required skills are available, then use only the matching skill |
| Environment bootstrap or missing toolchain | `docs/development/engineering/environment-setup.md` |
| BSP, pins, buses, display, audio, battery | `docs/hardware-design/AI_HARDWARE_DEVELOPMENT_GUIDE.md`, `components/bsp/include/bsp_pins.h` |
| Upstream demo or menu maintenance | `main/demo.h`, `main/main.c`, the nearest `main/demo_*.c` implementation |
| Chinese UI text or fonts | `docs/development/engineering/lvgl-chinese-fonts.md`, the application's font assets, configuration, and widget styles |
| Wi-Fi connection or Bluetooth provisioning | `docs/development/engineering/wifi-provisioning.md`, the referenced `demo/blufi-provisioning` implementation |
| Build, test, dependencies, partitions | `docs/development/engineering/build-and-test.md`, `docs/development/engineering/firmware-layout.md`, `sdkconfig.defaults`, `partitions.csv` |
| CI or release | the matching file in `docs/development/ci/CI-*.md` and `.github/workflows/` |
| Project completion | `docs/development/release/project-completion.md` (then the `issue-suggestions` or `experience-pr` skill) |
| Documentation | `docs/contribution/doc-conventions.md`, `docs/README.md` |
| Commit or PR | `docs/contribution/commit-and-pr.md` |

Use `docs/README.md` for the upstream product overview and the documentation index. The detailed AI development workflow — context setup, source-of-truth priority, application/BSP boundary, runtime invariants, material placement, and delivery format — is in `docs/development/ai-guide.md`. This fork's repository model is in `docs/fork-guide.md`.

## Required validation and delivery

Run the smallest relevant check while iterating, then run the complete gate before delivery:

```bash
./tools/validate.sh --static    # repository checks + host tests
./tools/validate.sh --firmware  # ESP-IDF build + merged-image verification
./tools/validate.sh             # complete gate
```

The complete gate requires an activated ESP-IDF 5.5.3 environment. The upstream gate does not discover fork tests: run `./jerryicat/tools/validate.sh` as well. Do not describe a successful build as hardware validation. Final delivery must report these fields separately:

```text
Build: PASS / FAIL / NOT RUN
Host tests: PASS / FAIL / NOT RUN
Device tests: PASS / FAIL / NOT RUN
Unverified: remaining board, instrument, or user checks
```

After each complete implementation of a user-requested firmware change,
proactively ask whether to flash it to the device for testing; do not wait for
a release request. If no device is detected, ask the user to turn it on and
connect it to a computer USB port with a data-capable cable. Follow the
[on-device testing handoff](docs/development/ai-guide.md#offer-on-device-testing)
and obtain approval before flashing; detection alone is not consent.

Ordinary upstream feature, application, and documentation changes must not edit `docs/CHANGELOG.md` or `docs/CHANGELOG.zh_CN.md`; describe user-visible behavior, compatibility, and release-flow impact in the pull-request body and authoritative documentation instead. This fork's product documentation belongs in `jerryicat/docs/`; change both changelog files only during an explicit release preparation.

Community guidance is in `.github/CONTRIBUTING.md`, `.github/CODE_OF_CONDUCT.md`, `.github/SECURITY.md`, and `.github/SUPPORT.md`.
