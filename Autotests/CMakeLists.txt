include(FetchContent)

message(STATUS "Fetching GTest.....")

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
FetchContent_MakeAvailable(googletest)

message(STATUS "Fetching nlohmann json.....")

FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

add_library(TradingEngine STATIC
        ../TradingEngine/src/AbstractOrderManager.cpp
        ../TradingStrategies/IntraExchangeArbitration/src/Bellman_Ford.cpp
)

target_include_directories(TradingEngine PRIVATE
        ${nlohmann_json_SOURCE_DIR}/include
        ${OPENSSL_INCLUDE_DIR}
        ${CURL_INCLUDE_DIRS}
)

target_link_libraries(TradingEngine PRIVATE nlohmann_json::nlohmann_json)

add_library(TESTING STATIC ../Logger/src/Logger.cpp)

add_executable(tests
        src/Run_all_tests.cpp
        src/test_tradingEngine.cpp
        src/test_BellmanFord.cpp
)

target_link_libraries(tests PRIVATE
        TradingEngine
        TESTING
        gtest
        gtest_main
        nlohmann_json::nlohmann_json
        OpenSSL::SSL
        OpenSSL::Crypto
        CURL::libcurl
)

target_include_directories(tests PRIVATE
        ${nlohmann_json_SOURCE_DIR}/include
        ${OPENSSL_INCLUDE_DIR}
        ${CURL_INCLUDE_DIRS}
        ../TradingEngine/include
)

enable_testing()

add_test(NAME TESTING COMMAND tests)
