cmake_minimum_required(VERSION 3.14)

project(TradingEngineTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Пути к сторонним библиотекам
set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/../third_party")

set(NLOHMANN_JSON_ROOT "${THIRD_PARTY_DIR}/nlohmann_json")
set(NLOHMANN_JSON_INCLUDE_DIR "${NLOHMANN_JSON_ROOT}") # Файл json.hpp лежит в этой папке

set(GTEST_ROOT "${THIRD_PARTY_DIR}/googletest/install")
set(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include")
set(GTEST_LIBRARY_DIR "${GTEST_ROOT}/lib")

find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

# Добавляем библиотеку TradingEngine
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

# Добавляем nlohmann_json как INTERFACE библиотеку (если это только заголовки)
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE ${NLOHMANN_JSON_INCLUDE_DIR})

target_link_libraries(TradingEngine PRIVATE nlohmann_json)

# Добавляем библиотеку TESTING
add_library(TESTING STATIC ../Logger/src/Logger.cpp)

# Создаем исполняемый файл для тестов
add_executable(tests
        src/Run_all_tests.cpp
        src/test_tradingEngine.cpp
        src/test_BellmanFord.cpp
)

target_include_directories(tests PRIVATE
        ${NLOHMANN_JSON_INCLUDE_DIR}
        ${GTEST_INCLUDE_DIR}
        ${OPENSSL_INCLUDE_DIR}
        ${CURL_INCLUDE_DIRS}
        ../TradingEngine/include
)

target_link_directories(tests PRIVATE
        ${GTEST_LIBRARY_DIR}
)

target_link_libraries(tests PRIVATE
        TradingEngine
        TESTING
        ${GTEST_LIBRARY_DIR}/libgtest.a
        ${GTEST_LIBRARY_DIR}/libgtest_main.a
        nlohmann_json
        OpenSSL::SSL
        OpenSSL::Crypto
        CURL::libcurl
)

enable_testing()

add_test(NAME TESTING COMMAND tests)
