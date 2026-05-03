include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG v1.15.2
)
# Prevent googletest from overriding our compiler/linker settings on Windows
# cmake-lint: disable=C0103
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(ohal_host_tests main.cpp test_register.cpp test_bitfield.cpp test_gpio.cpp
                               test_gpio_stm32u083kcu.cpp test_gpio_stm32u083hcy.cpp
                               test_gpio_stm32u083cci.cpp test_gpio_stm32u083cct.cpp)
target_link_libraries(ohal_host_tests PRIVATE ohal GTest::gtest_main GTest::gmock)

# RC-model tests use a separate executable because the STM32U083RC packages bond
# out GPIOD, giving Pin<PortD> a different definition than the KCU/HCY models.
# Mixing those definitions in a single executable would be an ODR violation.
add_executable(ohal_host_tests_rc main.cpp test_gpio_stm32u083rct.cpp
                                  test_gpio_stm32u083rci.cpp)
target_link_libraries(ohal_host_tests_rc PRIVATE ohal GTest::gtest_main GTest::gmock)

include(GoogleTest)
gtest_discover_tests(ohal_host_tests)
gtest_discover_tests(ohal_host_tests_rc)
