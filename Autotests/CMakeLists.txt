include(FetchContent)

message(STATUS "Fetching Gtest.....")

# Fetch Google Test
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
FetchContent_MakeAvailable(googletest)

# Fetch SQLite
FetchContent_Declare(
        sqlite3
        URL https://www.sqlite.org/2023/sqlite-amalgamation-3420000.zip
)
FetchContent_MakeAvailable(sqlite3)

# Create the sqlite3 library
add_library(sqlite3 STATIC ${sqlite3_SOURCE_DIR}/sqlite3.c)
target_include_directories(sqlite3 PUBLIC ${sqlite3_SOURCE_DIR})

# Link DatabaseWrapper library or add its source files
add_library(DatabaseWrapper STATIC ../Database/src/DatabaseWrapper.cpp ../Logger/src/Logger.cpp)  # Укажите правильный путь
target_include_directories(DatabaseWrapper PUBLIC ../Database/include)
target_link_libraries(DatabaseWrapper PRIVATE sqlite3)

# Create the test executable
add_executable(test_ src/Run_all_tests.cpp src/test_databaseWrapper.cpp)

# Link test executable to required libraries
target_link_libraries(test_ PRIVATE DatabaseWrapper sqlite3 gtest gtest_main)

# Enable testing
enable_testing()

add_test(NAME DatabaseWrapperTest COMMAND test_)
