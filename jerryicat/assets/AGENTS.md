<p align="right">
  <a href="AGENTS.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# assets/ Rules

This folder holds fork-specific binary assets: fonts, images, and audio used by
the launcher, plays, and on-device tools. Upstream reusable assets stay in the
repository-root `assets/`; these are ours.

## Layout and naming

- `fonts/`, `images/`, `audio/`. Do not create other top-level folders without a
  second file to justify them.
- Lowercase-kebab file names with the variant in the name, for example
  `launcher-title-20.bin` or `ok-click.wav`.
- Keep generated files and their sources together: a font's license and the
  conversion command belong beside it, not in a wiki memory.

## Provenance

Every asset records, in [`README.md`](README.md) (create the pair with the first
asset):

- Source URL, author or foundry, and license, with a statement that the license
  permits redistribution in this repository.
- The converter or tool name and version, and the exact command used.
- The character set or dimensions, sample rate, channels, bit depth, and other
  parameters that define the output.
- A checksum of the committed binary so a regenerated file can be compared.

Do not commit an asset whose license cannot be verified.

## Fonts

- Target LVGL 9.5 and the pinned `dependencies.lock` version.
- Prefer a subset at the sizes the UI actually uses over a full font family.
- Verify glyph coverage with `lv_font_get_glyph_dsc()` for every required code
  point; a placeholder is not coverage. Record the tested character set in
  [`../docs/`](../docs/AGENTS.md) or the consuming app's notes.
- Keep the source font when the license allows it; otherwise record where it came
  from and how to regenerate the subset.

## Images and audio

- The display is 240x320 portrait, RGB565. Size images for their actual placement
  and avoid full-screen buffers that do not fit the RAM budget.
- Audio must match the BSP codec and I2S configuration; record sample rate,
  channels, and format. Do not ship copyrighted music without a clear license.
- No credentials, device QR codes, personal photos, or unsanitized screenshots.

## Documentation

- Binary assets never mix with Markdown. This folder's README is the ledger;
  design notes live in `../docs/`.
- The ledger is a paired English/Chinese document like every other maintained
  Markdown file.
