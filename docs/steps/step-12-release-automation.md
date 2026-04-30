# Step 12 – Release Automation

**Goal:** Establish the release management, commit message, and merge workflow processes
*before* the platform and peripheral expansion work begins (Steps 14 and 15). Having these
processes in place ensures that every contributor — whether adding a new MCU family or a new
peripheral — automatically produces a well-structured history, correct changelogs, and correctly
versioned releases.

## 11.1 Conventional Commit Messages

All commits to the `main` branch must conform to the
[Conventional Commits](https://www.conventionalcommits.org/) specification. This is enforced on
the **PR title**, which becomes the squash-merge commit message.

### Required Types

| Type | When to use |
|---|---|
| `feat` | New peripheral, new MCU family/model, new public API |
| `fix` | Bug fix in an existing abstraction or platform file |
| `docs` | Documentation-only changes (plan, README, step files) |
| `test` | Adding or updating tests without changing production code |
| `ci` | Changes to GitHub Actions workflows or CI configuration |
| `build` | Changes to CMakeLists, toolchain files, or vcpkg manifests |
| `refactor` | Code restructuring with no observable behaviour change |
| `chore` | Maintenance (dependency bumps, generated file updates) |

### Scopes

Use scopes to identify the affected area:

| Scope | Meaning |
|---|---|
| `core` | `Register<>`, `BitField<>`, `Access` enum |
| `gpio` | GPIO peripheral interface or any platform's GPIO implementation |
| `spi`, `i2c`, `uart`, `timer`, `adc`, `dac`, `dma`, `power`, `clock`, `mpu` | Named peripheral |
| `stm32u0`, `pic`, `ti_mspm0`, `nrf5`, `rp2040`, `avr`, `samd` | MCU family |
| `ci` | Workflow files |
| `vcpkg` | Package manifest or port files |

Example well-formed PR titles:

```
feat(gpio): add STM32U073 partial specialisation

feat(stm32u0): add SPI1 register map and BitField specialisations

fix(core): correct mask calculation for Width=1 BitField at Offset=31

docs(plan): add step-15 additional peripherals

ci: add nRF5 cross-compile job
```

### Enforcement Mechanism

Add a GitHub Actions workflow that validates the PR title on every `pull_request` event:

```yaml
# .github/workflows/conventional-commits.yml
name: Conventional Commits

on:
  pull_request:
    types: [opened, edited, synchronize, reopened]

jobs:
  check-title:
    runs-on: ubuntu-latest
    steps:
      - uses: amannn/action-semantic-pull-request@v5
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
        with:
          types: |
            feat
            fix
            docs
            test
            ci
            build
            refactor
            chore
          requireScope: false
```

## 11.2 Merge Queue

The merge queue serialises PR merges so that the `main` branch is always in a green state.
Each PR must pass CI before it can join the queue; the queue itself re-runs CI against the
current `main` before completing the merge.

### Repository Settings (manual, one-time)

In **Settings → General → Pull Requests**:
- Enable **Require merge queue**.
- Set **Merge method**: Squash and merge (so the PR title becomes the single commit message).

### Branch Protection Rule for `main`

In **Settings → Branches → main**:
- Require status checks to pass before merging (select all CI jobs from Step 11).
- Require merge queue.
- Do not allow bypassing the above settings.

### Merge Queue Workflow Job

The merge queue triggers CI via the `merge_group` event. Update every CI workflow to respond
to this event:

```yaml
on:
  push:
    branches: [main]
  pull_request:
  merge_group:        # ← add this trigger to all CI workflows
```

## 11.3 release-please

[release-please](https://github.com/googleapis/release-please) reads the conventional commit
history and automatically:

1. Opens a "Release PR" that bumps the version in `vcpkg.json`, `ports/ohal/vcpkg.json`,
   `CMakeLists.txt` (`project(ohal VERSION ...)`), and updates `CHANGELOG.md`.
2. Merges the Release PR (after CI passes through the merge queue).
3. Creates a GitHub Release with a tag (e.g. `v0.2.0`) and auto-generated release notes.

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
      - uses: googleapis/release-please-action@v4
        with:
          release-type: simple
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

## 11.4 Logical Dependency on Later Steps

- [Step 13 (vcpkg)](step-13-vcpkg-package.md) depends on this step: `release-please` must be
  configured before `vcpkg.json` is published, because the version field is bumped by
  `release-please` automatically.
- [Step 14 (Additional MCU Families)](step-14-additional-mcu-families.md) and
  [Step 15 (Additional Peripherals)](step-15-additional-peripherals.md) both generate
  `feat(...)` commits; having conventional commit enforcement in place before those PRs are
  raised ensures the changelog and release notes are meaningful from the start.
