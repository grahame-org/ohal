#!/usr/bin/env bash
# lint.sh — runs all linting and formatting checks.
# Exit codes: 0 = all checks passed, non-zero = at least one check failed.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "=== prettier ==="
find "${REPO_ROOT}" \
    \( -name '*.md' -o -name '*.yml' -o -name '*.yaml' -o -name '*.json' \) \
    ! -path "${REPO_ROOT}/.git/*" \
    -print0 | xargs -0 npx prettier --check

echo "=== clang-format ==="
find "${REPO_ROOT}/include" "${REPO_ROOT}/tests" \
    \( -name '*.hpp' -o -name '*.cpp' \) \
    -print0 | xargs -0 clang-format --dry-run --Werror

echo "=== clang-tidy ==="
find "${REPO_ROOT}/include" -name '*.hpp' \
    -print0 | xargs -0 -I{} clang-tidy \
    --extra-arg="-std=c++17" \
    --extra-arg="-I${REPO_ROOT}/include" \
    --extra-arg="-DOHAL_FAMILY_STM32U0" \
    --extra-arg="-DOHAL_MODEL_STM32U083" \
    {} --

echo "=== cmake-lint ==="
find "${REPO_ROOT}" \
    \( -name 'CMakeLists.txt' -o -name '*.cmake' -o -name '*.cmake.in' \) \
    -print0 | xargs -0 cmake-lint

echo "=== yamllint ==="
yamllint -c "${REPO_ROOT}/.yamllint.yml" "${REPO_ROOT}/.github/workflows"

echo "=== spec validation ==="
while IFS= read -r -d '' spec; do
    check-jsonschema --schemafile "${REPO_ROOT}/docs/specs/schema.json" "${spec}"
done < <(find "${REPO_ROOT}/docs/specs" -mindepth 2 -maxdepth 2 -name '*.yml' -print0)

echo "=== model spec validation ==="
while IFS= read -r -d '' spec; do
    check-jsonschema --schemafile "${REPO_ROOT}/docs/specs/schema-model.json" "${spec}"
done < <(find "${REPO_ROOT}/docs/specs" -mindepth 3 -maxdepth 3 -name '*.yml' -path '*/models/*' -print0)

echo "=== arch spec validation ==="
while IFS= read -r -d '' spec; do
    check-jsonschema --schemafile "${REPO_ROOT}/docs/specs/schema-arch.json" "${spec}"
done < <(find "${REPO_ROOT}/docs/specs/common/arch" -name '*.yml' -print0)

echo "=== shellcheck ==="
find "${REPO_ROOT}" -name '*.sh' -print0 | xargs -0 shellcheck

echo "=== markdownlint ==="
MARKDOWNLINT_TARGETS=("${REPO_ROOT}/docs/**/*.md" "${REPO_ROOT}/README.md")
if [ -f "${REPO_ROOT}/CHANGELOG.md" ]; then
    MARKDOWNLINT_TARGETS+=("${REPO_ROOT}/CHANGELOG.md")
fi
markdownlint-cli2 "${MARKDOWNLINT_TARGETS[@]}"

echo "All lint checks passed."
