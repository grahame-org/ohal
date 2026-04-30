<!--
PR title MUST follow Conventional Commits format:

  <type>(<scope>): <short summary>

  type  : feat | fix | docs | test | ci | build | refactor | chore
  scope : optional — e.g. core | gpio | stm32u0 | pic | ci | vcpkg
  summary: present tense, lowercase, no trailing period

Examples:
  feat(gpio): add STM32U073 partial specialisation
  fix(core): correct mask calculation for Width=1 BitField at Offset=31
  docs(plan): add step-16 additional peripherals
  ci: add nRF5 cross-compile job

The PR title becomes the squash-merge commit message and is read by
release-please to determine version bumps and CHANGELOG entries.
-->
