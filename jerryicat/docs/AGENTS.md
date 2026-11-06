<p align="right">
  <a href="AGENTS.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# docs/ Rules

This folder records how this fork's product is designed and why. It is
fork-private documentation, not a copy of the upstream guides.

## What goes here

- Architecture notes for the launcher, shared UI kit, and app lifecycle.
- Play and on-device-tool specifications that outgrow a folder `AGENTS.md`.
- Product decisions, tradeoffs, data formats, and reset/versioning rules.
- Device test records for fork firmware: build identity, what was observed, and
  what remains unverified.

Upstream hardware, BSP, engineering, and contribution rules stay in `../../docs/`;
link to them instead of restating them.

## Language and structure

- English at `name.md`, Simplified Chinese at `name.zh_CN.md`, with reciprocal
  language links at the top of both files.
- Lowercase-kebab file names; one topic per document. Do not create a document
  per screen or per commit.
- Register every document in [`../README.md`](../README.md) or a docs index, and
  update links when files move.
- Keep diagrams text-based (ASCII or Mermaid) unless a rendered image is
  essential; binary images belong in `../assets/`.

## Writing rules

- State the decision, the constraint behind it, and the failure mode it prevents.
  Do not keep a debugging diary or transient notes.
- Quote exact build identities and device observations for test records; label
  inference as inference.
- No credentials, device secrets, personal data, private URLs, or unsanitized
  logs.
- Keep the two languages aligned in the same change; do not let one side drift.
