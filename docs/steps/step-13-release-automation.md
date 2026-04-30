# Step 13 – Release Automation

**Goal:** Configure `release-please` to automatically track the conventional commit history
accumulated since [Step 3](step-03-conventional-commits-merge-queue.md) and produce versioned
GitHub Releases, changelog entries, and version bumps in `vcpkg.json` and
`ports/ohal/vcpkg.json`.

**Prerequisites:** [Step 3 (Conventional Commits and Merge Queue)](step-03-conventional-commits-merge-queue.md)
must be in place so that the commit history `release-please` reads is well-formed.
[Step 12 (CI)](step-12-ci.md) must be in place so the merge queue has status checks to run
before completing each release PR merge.

## 13.1 release-please

[release-please](https://github.com/googleapis/release-please) reads the conventional commit
history and automatically:

1. Opens or updates a "Release PR" that bumps the version in `vcpkg.json`,
   `ports/ohal/vcpkg.json`, and updates `CHANGELOG.md`.
2. After the Release PR is merged (via the merge queue), creates a GitHub Release with a
   tag (e.g. `v0.2.0`) and auto-generated release notes.

### Configuration

```yaml
# .github/workflows/release-please.yml
name: release-please

on:
  push:
    branches: [main]

permissions:
  contents: write
  pull-requests: write

jobs:
  release-please:
    runs-on: ubuntu-latest
    steps:
      # Pin GitHub Actions to a full commit SHA; update this SHA intentionally when upgrading.
      - uses: googleapis/release-please-action@<FULL_40_CHARACTER_COMMIT_SHA>
        with:
          command: manifest
          config-file: release-please-config.json
          manifest-file: .release-please-manifest.json
          token: ${{ secrets.GITHUB_TOKEN }}
```

### `release-please-config.json`

```json
{
  "release-type": "simple",
  "packages": {
    ".": {
      "release-type": "simple",
      "extra-files": [
        {
          "type": "json",
          "path": "vcpkg.json",
          "jsonpath": "$.version"
        },
        {
          "type": "json",
          "path": "ports/ohal/vcpkg.json",
          "jsonpath": "$.version"
        }
      ]
    }
  }
}
```

### `.release-please-manifest.json`

```json
{
  ".": "0.1.0"
}
```

## 13.2 Logical Dependency on Later Steps

- [Step 14 (vcpkg)](step-14-vcpkg-package.md) depends on this step: `release-please` must be
  configured before `vcpkg.json` is published, because the version field is bumped by
  `release-please` automatically.
- [Step 15 (Additional MCU Families)](step-15-additional-mcu-families.md) and
  [Step 16 (Additional Peripherals)](step-16-additional-peripherals.md) both generate
  `feat(...)` commits; having `release-please` in place before those PRs are raised ensures the
  changelog and release notes are meaningful and correctly versioned from the start.
