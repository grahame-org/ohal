export default {
  extends: ["@commitlint/config-conventional"],
  rules: {
    "type-enum": [2, "always", ["feat", "fix", "docs", "test", "ci", "build", "refactor", "chore"]],
  },
};
