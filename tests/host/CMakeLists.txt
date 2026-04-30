include(FetchContent)

FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest
    GIT_TAG        v2.4.11
)
FetchContent_MakeAvailable(doctest)

add_executable(ohal_host_tests main.cpp)
target_link_libraries(ohal_host_tests PRIVATE ohal doctest::doctest)

add_test(NAME ohal_host_tests COMMAND ohal_host_tests)
