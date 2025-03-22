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
        src/KrakenOrderManager.cpp
        src/OKXOrderManager.cpp
        src/TradingServer.cpp
        src/BinanceOrderManager.cpp
        src/AbstractOrderManager.cpp
        ../DataCollector/src/DataCollector.cpp
        ../Logger/src/CSVLogger.cpp
)

add_executable(server ${SOURCES})
find_package(OpenSSL REQUIRED)

target_link_libraries(server
        PRIVATE
        nlohmann_json::nlohmann_json
        jsoncpp_lib
        curl
        OpenSSL::SSL
        OpenSSL::Crypto
        StrategiesAPI
)


target_include_directories(server
        PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}
)


message(STATUS "CMake configuration is complete.")
