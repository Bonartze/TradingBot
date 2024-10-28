include(FetchContent)

message(STATUS "Fetching SQLite3...")

FetchContent_Declare(
        sqlite3
        URL "https://www.sqlite.org/2022/sqlite-amalgamation-3400100.zip"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(sqlite3)

add_library(sqlite3 SHARED "${sqlite3_SOURCE_DIR}/sqlite3.c")

target_include_directories(sqlite3 PUBLIC "${sqlite3_SOURCE_DIR}")

add_library(DatabaseWrapper DatabaseWrapper.cpp)

target_include_directories(DatabaseWrapper PRIVATE "${sqlite3_SOURCE_DIR}")