<p align="right">
  <a href="CI-sync-main.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# Upstream Synchronization

`.github/workflows/sync-main.yml` can synchronize a fork's `main` branch with `FoloToy/ai-passport:main`. This fork develops directly on `main` and rewrites fork-owned documents, so automatic synchronization is disabled: the workflow runs only on manual dispatch, and only when the user asks for an upstream sync.

The workflow checks out the target `main` without persisting credentials and uses the full-SHA-pinned upstream-sync Action. Its generated `GITHUB_TOKEN` has only `contents: write`; no manually configured token is required. If synchronization fails after an upstream workflow change, use GitHub's **Sync fork** once and inspect the Actions log.

Run the sync by hand only after confirming the user wants it. Expect conflicts in `AGENTS.md`, `docs/`, and other fork-owned files; resolve them in favor of the fork rules and never drop `jerryicat/` work to take an upstream version wholesale. Fork owners must explicitly enable Actions after forking.

The checkout explicitly sets `ref: main`, even when a manual dispatch selects
another branch. The sync Action expects a local `main` branch before fetching;
without that explicit checkout, `git checkout main` can interpret this
repository's `main/` directory as a path instead of switching branches.

When upgrading an Action, verify the release and full commit SHA from the official repository and update the inline version comment.
