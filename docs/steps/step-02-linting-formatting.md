# Step 2 – Linting and Formatting Enforcement

**Goal:** Introduce a single `lint.sh` entry-point that runs every linter and formatter check.
A single CI job calls this script, so future additions to linting never require updating the
required status checks list in the branch protection rule.

A separate `zizmor` workflow audits the GitHub Actions workflow files themselves for security
issues on every pull request, merge queue entry, and push to `main`.

## 2.1 Design Principle: One Script, One Required Check

The branch protection rule lists exactly **one** lint-related required status check: `lint`.
That CI job simply runs `./lint.sh`. When a new tool is added (for example `lizard` for
cyclomatic-complexity analysis), only `lint.sh` and its tool-installation step are updated.
The branch protection required-checks list stays unchanged.

```
lint.sh ──► clang-format   (C++ style)
        ──► clang-tidy     (C++ static analysis)
        ──► cmake-lint     (CMake style)
        ──► yamllint       (YAML / workflow files)
        ──► shellcheck     (shell scripts)
        ──► markdownlint   (documentation)
```

## 2.2 `lint.sh`

Place `lint.sh` at the repository root. The script is the single source of truth for what
"passing lint" means. CI and local developer runs both invoke this file.

```bash
#!/usr/bin/env bash
# lint.sh — runs all linting and formatting checks.
# Exit codes: 0 = all checks passed, non-zero = at least one check failed.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "=== clang-format ==="
find "${REPO_ROOT}/include" "${REPO_ROOT}/tests" \
    \( -name '*.hpp' -o -name '*.cpp' \) \
    -print0 | xargs -0 clang-format --dry-run --Werror

echo "=== clang-tidy ==="
find "${REPO_ROOT}/include" -name '*.hpp' \
    -print0 | xargs -0 clang-tidy \
    --extra-arg="-std=c++17" \
    --extra-arg="-I${REPO_ROOT}/include"

echo "=== cmake-lint ==="
find "${REPO_ROOT}" \
    \( -name 'CMakeLists.txt' -o -name '*.cmake' -o -name '*.cmake.in' \) \
    -print0 | xargs -0 cmake-lint

echo "=== yamllint ==="
yamllint -c "${REPO_ROOT}/.yamllint.yml" "${REPO_ROOT}/.github/workflows"

echo "=== shellcheck ==="
find "${REPO_ROOT}" -name '*.sh' -print0 | xargs -0 shellcheck

echo "=== markdownlint ==="
markdownlint-cli2 \
    "${REPO_ROOT}/docs/**/*.md" \
    "${REPO_ROOT}/README.md" \
    "${REPO_ROOT}/CHANGELOG.md"

echo "All lint checks passed."
```

## 2.3 Tool Configuration Files

| File | Tool | Purpose |
|---|---|---|
| `.clang-format` | clang-format | C++ style: LLVM base, column limit 100, pointer alignment Left |
| `.clang-tidy` | clang-tidy | Enabled checks: `cppcoreguidelines-*`, `modernize-*`, `readability-*`, `bugprone-*`, `performance-*`; header filter `^include/ohal/` |
| `.yamllint.yml` | yamllint | 2-space indent, max line length 120, require document start marker |
| `.markdownlint.json` | markdownlint-cli2 | MD013 line length 120, MD033 inline HTML off |

### `.clang-format` (sketch)

```yaml
BasedOnStyle: LLVM
ColumnLimit: 100
PointerAlignment: Left
AlwaysBreakTemplateDeclarations: Yes
SortIncludes: CaseInsensitive
```

### `.clang-tidy` (sketch)

```yaml
Checks: >
  cppcoreguidelines-*,
  modernize-*,
  readability-*,
  bugprone-*,
  performance-*,
  -modernize-use-trailing-return-type
HeaderFilterRegex: '^include/ohal/'
WarningsAsErrors: '*'
```

### `.yamllint.yml` (sketch)

```yaml
extends: default
rules:
  line-length:
    max: 120
  document-start: enable
  indentation:
    spaces: 2
```

### `.markdownlint.json` (sketch)

```json
{
  "MD013": { "line_length": 120 },
  "MD033": false
}
```

## 2.4 CI Workflow: `lint.yml`

```yaml
# .github/workflows/lint.yml
name: Lint

on:
  pull_request:
  merge_group:

jobs:
  lint:
    name: lint
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Install tools
        run: |
          sudo apt-get update -y
          sudo apt-get install -y clang-format clang-tidy cmake shellcheck
          pip3 install cmakelang yamllint
          npm install -g markdownlint-cli2

      - name: Run lint.sh
        run: bash lint.sh
```

The `jobs.lint.name` field is `lint`. This value is the string configured as the required
status check in the branch protection rule and the merge queue. It **must not be renamed**
without a corresponding branch protection update.

### Triggers

| Event | Reason |
|---|---|
| `pull_request` | Enforce style on every proposed change before review. |
| `merge_group` | Re-enforce when the PR is batched with the current `main` tip before merging. |

`push` to `main` is intentionally omitted: the merge queue ensures that nothing reaches `main`
without passing lint first.

## 2.5 Zizmor Workflow: `zizmor.yml`

[zizmor](https://github.com/woodruffw/zizmor) audits GitHub Actions workflow files for
security vulnerabilities (script injection, excessive permissions, unpinned actions,
credential capture via artifacts, and more). The `--pedantic` flag activates every available
check, including advisory-level findings.

Results are uploaded as SARIF and appear in the repository's **Security → Code scanning** tab.

```yaml
# .github/workflows/zizmor.yml
name: zizmor

on:
  pull_request:
  merge_group:
  push:
    branches: [main]

permissions:
  contents: read
  security-events: write    # required to upload SARIF to Code Scanning

jobs:
  zizmor:
    name: zizmor
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
        with:
          persist-credentials: false

      - name: Install zizmor
        run: pip3 install zizmor

      - name: Run zizmor (pedantic)
        run: zizmor --pedantic --format sarif . > zizmor-results.sarif

      - name: Upload SARIF results
        uses: github/codeql-action/upload-sarif@v3
        if: always()
        with:
          sarif_file: zizmor-results.sarif
          category: zizmor
```

### Why `--pedantic`?

The `--pedantic` flag activates every available audit, including:

| Check | What it finds |
|---|---|
| `artipacked` | Credentials that could be captured by a workflow artifact upload |
| `ref-confusion` | Ambiguous `ref:` values in `actions/checkout` steps |
| `excessive-permissions` | `permissions:` grants that are broader than the job requires |
| `unpinned-uses` | `uses:` references with tag or branch names instead of full commit-SHA pins |
| `pull-request-target` | Unsafe use of `pull_request_target` with code from the fork |
| `template-injection` | GitHub expression values used directly in `run:` without sanitisation |

All findings must be resolved — or explicitly suppressed with a `# zizmor-ignore[<rule>]`
comment accompanied by a written justification — before the PR can merge.

### Triggers

| Event | Reason |
|---|---|
| `pull_request` | Catch newly introduced workflow-security issues in proposed changes. |
| `merge_group` | Re-audit as the PR is batched against the current `main` tip. |
| `push` to `main` | Baseline scan of the full workflow set on every merge, feeds Code Scanning history. |

### Separate Required Status Check

`zizmor` runs as a **separate** required status check (check name: `zizmor`) rather than being
included in `lint.sh`. This separation exists because:

- The SARIF upload step requires `security-events: write` permission, which `lint.yml` does
  not (and should not) hold.
- Security findings and style findings have different triage processes.
- Suppression of a zizmor finding (`# zizmor-ignore`) is a security decision that benefits
  from a distinct review trail.

## 2.6 Adding Future Linting or Formatting Tools

1. Add the tool's installation command to the `Install tools` step in `lint.yml`
   (or pre-install it in a shared runner image / dev container).
2. Add the invocation to `lint.sh`.
3. **No changes to branch protection required checks are needed**: the single `lint` job
   already covers the new tool by calling `lint.sh`.

## 2.7 Dependency on Later Steps

`lint.sh` and all configuration files (`.clang-format`, `.clang-tidy`, etc.) are established
in this step so that every implementation step from Step 3 onward produces lint-compliant code
from the first commit. The CI workflows (`lint.yml`, `zizmor.yml`) are also created here; they
become active as soon as the repository has workflows to run.

- [Step 11 (CI)](step-11-ci.md): registers `lint` and `zizmor` as required status checks in
  the branch protection rule alongside the build and test jobs.
- [Step 12 (Release Automation)](step-12-release-automation.md): adds `lint` and `zizmor` to
  the merge queue configuration so that formatting and security findings block merges with the
  same authority as build failures.
