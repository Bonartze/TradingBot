include(FetchContent)

message(STATUS "Fetching Gtest.....")

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(test_ Run_all_tests.cpp test_databaseWrapper.cpp)

target_link_libraries(test_ PRIVATE DatabaseWrapper sqlite3 gtest gtest_main)


add_dependencies(test_ DatabaseWrapper)

enable_testing()

add_test(NAME DatabaseWrapperTest COMMAND test_)

