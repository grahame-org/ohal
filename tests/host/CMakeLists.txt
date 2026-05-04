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
                               test_gpio_stm32u083ccu.cpp test_gpio_stm32u083cct.cpp)
target_link_libraries(ohal_host_tests PRIVATE ohal GTest::gtest_main GTest::gmock)

# RC-model tests use separate executables: the STM32U083RC packages bond out
# GPIOD (giving Pin<PortD> a working definition), while the KCU/HCY models
# provide a static_assert for Pin<PortD>.  Mixing those in one executable would
# be an ODR violation.  RCT and RCI are kept separate so each binary contains
# only one set of explicit template specialisations.
add_executable(ohal_host_tests_rct main.cpp test_gpio_stm32u083rct.cpp)
target_link_libraries(ohal_host_tests_rct PRIVATE ohal GTest::gtest_main GTest::gmock)

add_executable(ohal_host_tests_rci main.cpp test_gpio_stm32u083rci.cpp)
target_link_libraries(ohal_host_tests_rci PRIVATE ohal GTest::gtest_main GTest::gmock)

# MC-model tests use separate executables: the STM32U083MC packages bond out
# GPIOE (giving Pin<PortE> a working definition), while the KCU/HCY/CC models
# provide a static_assert for Pin<PortE>.  Mixing those in one executable would
# be an ODR violation.  MCT and MCI are kept separate because they provide
# different Pin<PortE, 3> specialisations (absent on MCT, bonded on MCI).
add_executable(ohal_host_tests_mct main.cpp test_gpio_stm32u083mct.cpp)
target_link_libraries(ohal_host_tests_mct PRIVATE ohal GTest::gtest_main GTest::gmock)

add_executable(ohal_host_tests_mci main.cpp test_gpio_stm32u083mci.cpp)
target_link_libraries(ohal_host_tests_mci PRIVATE ohal GTest::gtest_main GTest::gmock)

include(GoogleTest)
gtest_discover_tests(ohal_host_tests)
gtest_discover_tests(ohal_host_tests_rct)
gtest_discover_tests(ohal_host_tests_rci)
gtest_discover_tests(ohal_host_tests_mct)
gtest_discover_tests(ohal_host_tests_mci)
