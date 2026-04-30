# Step 3 – Conventional Commits and Merge Queue

**Goal:** Enforce a well-formed, machine-parseable commit history and serialised merge
discipline before any implementation code is written. Every PR from Step 4 onward will be
squash-merged with a conventional commit title; having the validation and merge queue active
from the very first implementation PR means the git history is clean and correctly structured
by the time `release-please` (Step 13) begins reading it.

## 3.1 Conventional Commit Messages

All commits to the `main` branch must conform to the
[Conventional Commits](https://www.conventionalcommits.org/) specification. This is enforced on
the **PR title**, which becomes the squash-merge commit message.

### Required Types

| Type       | When to use                                                |
| ---------- | ---------------------------------------------------------- |
| `feat`     | New peripheral, new MCU family/model, new public API       |
| `fix`      | Bug fix in an existing abstraction or platform file        |
| `docs`     | Documentation-only changes (plan, README, step files)      |
| `test`     | Adding or updating tests without changing production code  |
| `ci`       | Changes to GitHub Actions workflows or CI configuration    |
| `build`    | Changes to CMakeLists, toolchain files, or vcpkg manifests |
| `refactor` | Code restructuring with no observable behaviour change     |
| `chore`    | Maintenance (dependency bumps, generated file updates)     |

### Scopes

Use scopes to identify the affected area:

| Scope                                                                       | Meaning                                                         |
| --------------------------------------------------------------------------- | --------------------------------------------------------------- |
| `core`                                                                      | `Register<>`, `BitField<>`, `Access` enum                       |
| `gpio`                                                                      | GPIO peripheral interface or any platform's GPIO implementation |
| `spi`, `i2c`, `uart`, `timer`, `adc`, `dac`, `dma`, `power`, `clock`, `mpu` | Named peripheral                                                |
| `stm32u0`, `pic`, `ti_mspm0`, `nrf5`, `rp2040`, `avr`, `samd`               | MCU family                                                      |
| `ci`                                                                        | Workflow files                                                  |
| `vcpkg`                                                                     | Package manifest or port files                                  |

Example well-formed PR titles:

```text
feat(gpio): add STM32U073 partial specialisation

feat(stm32u0): add SPI1 register map and BitField specialisations

fix(core): correct mask calculation for Width=1 BitField at Offset=31

docs(plan): add step-16 additional peripherals

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

## 3.2 Merge Queue

The merge queue serialises PR merges so that the `main` branch is always in a green state.
Each PR must pass its required checks before it can join the queue; the queue itself re-runs
the required checks against the current `main` tip before completing the merge.

### Repository Settings (manual, one-time)

In **Settings → General → Pull Requests**:

- Enable **Require merge queue**.
- Set **Merge method**: Squash and merge (so the PR title becomes the single commit message).

### Branch Protection Rule for `main`

In **Settings → Branches → main**:

- Require status checks to pass before merging.
- Require merge queue.
- Do not allow bypassing the above settings.

The required status checks will be populated incrementally:

- After Step 2: add `lint` and `zizmor`.
- After Step 12: add all CI build/test job names.

### Merge Queue Workflow Trigger

The merge queue triggers workflows via the `merge_group` event. Every workflow that is listed
as a required status check must respond to this event:

```yaml
on:
  pull_request:
  merge_group: # ← required on every workflow registered as a status check
```

Workflows that are _not_ required status checks (e.g. `release-please.yml`, which only runs
on push to `main`) do not need this trigger.

## 3.3 Dependency on Later Steps

- [Step 12 (CI)](step-12-ci.md): all build and test job names are added to the branch
  protection required status checks list at this point.
- [Step 13 (Release Automation)](step-13-release-automation.md): `release-please` reads the
  conventional commit history accumulated from this step forward to determine version bumps and
  generate `CHANGELOG.md` entries. A clean conventional commit history from the first
  implementation PR makes this process accurate and reliable.
