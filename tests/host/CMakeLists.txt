include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG        v1.15.2
)
# Prevent googletest from overriding our compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(ohal_host_tests main.cpp)
target_link_libraries(ohal_host_tests PRIVATE ohal GTest::gtest_main GTest::gmock)

include(GoogleTest)
gtest_discover_tests(ohal_host_tests)
