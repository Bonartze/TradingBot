cmake_minimum_required(VERSION 3.14)

project(TradingEngineTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/../third_party")

set(NLOHMANN_JSON_ROOT "${THIRD_PARTY_DIR}/nlohmann_json")
set(NLOHMANN_JSON_INCLUDE_DIR "${NLOHMANN_JSON_ROOT}")

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
FetchContent_MakeAvailable(googletest)

find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

add_library(TradingEngine STATIC
        ../TradingEngine/src/AbstractOrderManager.cpp
        ../TradingStrategies/IntraExchangeArbitration/src/Bellman_Ford.cpp
)

target_include_directories(TradingEngine PRIVATE
        ${NLOHMANN_JSON_INCLUDE_DIR}
        ${OPENSSL_INCLUDE_DIR}
        ${CURL_INCLUDE_DIRS}
        ../TradingEngine/include
)

add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE ${NLOHMANN_JSON_INCLUDE_DIR})

target_link_libraries(TradingEngine PRIVATE nlohmann_json)

add_library(TESTING STATIC ../Logger/src/Logger.cpp)

add_executable(tests
        src/Run_all_tests.cpp
        src/test_tradingEngine.cpp
        src/test_BellmanFord.cpp
)

target_include_directories(tests PRIVATE
        ${NLOHMANN_JSON_INCLUDE_DIR}
        ${OPENSSL_INCLUDE_DIR}
        ${CURL_INCLUDE_DIRS}
        ../TradingEngine/include
)

target_link_libraries(tests PRIVATE
        TradingEngine
        TESTING
        gtest
        gtest_main
        nlohmann_json
        OpenSSL::SSL
        OpenSSL::Crypto
        CURL::libcurl
        pthread
)

enable_testing()
add_test(NAME TESTING COMMAND tests)
