cmake_minimum_required(VERSION 3.15)

project(TradingBot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
        jsoncpp
        GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
        GIT_TAG master
)
FetchContent_MakeAvailable(jsoncpp)

find_package(Boost 1.86.0 REQUIRED)

set(SOURCES
        src/TradingServer.cpp
)

add_library(OrderManager ${SOURCES})

target_link_libraries(OrderManager
        PRIVATE
        nlohmann_json::nlohmann_json
        jsoncpp_lib
)

target_include_directories(OrderManager
        PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${nlohmann_json_SOURCE_DIR}/include
)

target_compile_options(OrderManager PRIVATE -Wall -Wextra -Werror)

message(STATUS "CMake configuration is complete.")
