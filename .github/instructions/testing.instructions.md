---
applyTo: "tests/**"
---

# Testing policy

## One test, one assert

Every test function should contain **exactly one** `EXPECT_*` or `ASSERT_*` call.

**Rationale:** A test with a single assertion has one clear reason to fail. Multiple
assertions in a single test obscure which expectation is violated. When using
`ASSERT_*` (fatal assertions), a failure stops the test immediately, silently
skipping all subsequent assertions. Even with `EXPECT_*` (non-fatal), reporting
multiple unrelated failures from one test body makes it harder to pinpoint the
root cause.

### ✅ Correct

```cpp
TEST_F(Register32Test, WriteStoresValue) {
  Reg32::write(0xFEED'FACEu);
  EXPECT_EQ(reg32_storage, 0xFEED'FACEu);
}
```

### ❌ Incorrect

```cpp
TEST_F(BitField32Test, WritePreservesOtherBits) {
  RwField::write(0b101U);
  EXPECT_EQ(field32_storage & mask, expected_field_bits);  // first assert
  EXPECT_EQ(field32_storage & ~mask, expected_other_bits); // second assert — not allowed
}
```

Split the above into two tests, one per assertion.

---

## Use parameterised tests to eliminate duplication

Whenever the same test logic applies to multiple inputs (different register widths,
field configurations, expected values, …) use GTest's parameterisation mechanisms
instead of copy-pasting test bodies.

Choose the right mechanism for each situation:

| Situation                                                                 | Mechanism                             |
| ------------------------------------------------------------------------- | ------------------------------------- |
| Same logic, different **types** (e.g. `uint8_t` vs `uint32_t` registers)  | `TYPED_TEST_SUITE` + `TYPED_TEST`     |
| Same logic, different **values** at runtime (e.g. mask table, enum table) | `TEST_P` + `INSTANTIATE_TEST_SUITE_P` |

### `TYPED_TEST_SUITE` example (different register widths)

```cpp
struct Config32 {
  inline static uint32_t storage{0U};
  using Reg = ohal::test::MockRegister<uint32_t, &storage>;
  static constexpr uint32_t write_val = 0xFEED'FACEu;
  static constexpr uint32_t read_val  = 0xFADE'CAFEu;
  static void reset() noexcept { storage = 0U; }
};

struct Config8 {
  inline static uint8_t storage{0U};
  using Reg = ohal::test::MockRegister<uint8_t, &storage>;
  static constexpr uint8_t write_val = 0xABu;
  static constexpr uint8_t read_val  = 0xCDu;
  static void reset() noexcept { storage = 0U; }
};

template <typename Config>
class RegisterTest : public ::testing::Test {
protected:
  void SetUp() override { Config::reset(); }
};

using RegisterConfigs = ::testing::Types<Config32, Config8>;
TYPED_TEST_SUITE(RegisterTest, RegisterConfigs);

TYPED_TEST(RegisterTest, WriteStoresValue) {
  using Reg = typename TypeParam::Reg;
  Reg::write(TypeParam::write_val);
  EXPECT_EQ(static_cast<uint32_t>(TypeParam::storage), static_cast<uint32_t>(TypeParam::write_val));
}
```

### `TEST_P` example (value-parameterised, e.g. mask table)

```cpp
struct MaskCase {
  uint32_t actual_mask;
  uint32_t expected_mask;
  const char* name;
};

class BitFieldMaskTest : public ::testing::TestWithParam<MaskCase> {};

INSTANTIATE_TEST_SUITE_P(
    FieldMasks, BitFieldMaskTest,
    ::testing::Values(
        MaskCase{RwField::mask, 0b0111'0000U, "RwField"},
        MaskCase{RoField::mask, 0x0F00U,      "RoField"}),
    [](const ::testing::TestParamInfo<MaskCase>& info) { return info.param.name; });

TEST_P(BitFieldMaskTest, MaskIsCorrect) {
  const auto& p = GetParam();
  EXPECT_EQ(p.actual_mask, p.expected_mask);
}
```

---

## Fixture setup and teardown

- Reset all shared state in `SetUp()`, never in `TearDown()`. Resetting in
  `SetUp()` guarantees a clean slate even if a previous test was skipped.
- Prefer `::testing::Test` fixtures for stateful tests; avoid global `SetUp`
  inside `TEST()` bodies.

---

## Naming conventions

Test names must describe **what is being tested** and **what the expected outcome
is**, in the form `Subject_Scenario` or `Subject_Scenario_ExpectedOutcome`.

| ✅ Good                                   | ❌ Bad          |
| ----------------------------------------- | --------------- |
| `WriteStoresValue`                        | `Test1`         |
| `ReadWriteField_WritePreservesOtherBits`  | `CheckPreserve` |
| `WriteTruncatesExtraBits_FieldBitsAreSet` | `WriteTruncate` |
