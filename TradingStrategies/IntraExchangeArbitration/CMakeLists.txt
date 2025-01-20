cmake_minimum_required(VERSION 3.16)
project(MyTradingBot)

set(CMAKE_CXX_STANDARD 17)

# Настройка Boost
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.75 REQUIRED COMPONENTS json)

# Проверка, найден ли Boost::json
if (NOT Boost_JSON_FOUND)
    message(FATAL_ERROR "boost_json not found. Ensure Boost version is 1.75 or newer and properly installed.")
endif()

# Зависимости
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

# FetchContent для simdjson
include(FetchContent)
FetchContent_Declare(
        simdjson
        GIT_REPOSITORY https://github.com/simdjson/simdjson.git
        GIT_TAG v0.4.7
        GIT_SHALLOW TRUE
)
set(SIMDJSON_JUST_LIBRARY ON CACHE INTERNAL "")
set(SIMDJSON_BUILD_STATIC ON CACHE INTERNAL "")
FetchContent_MakeAvailable(simdjson)

# FetchContent для других библиотек
FetchContent_Declare(
        jsoncpp
        GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
        GIT_TAG master
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(jsoncpp)

# Указание директорий для заголовков
include_directories(
        ${CMAKE_SOURCE_DIR}/TradingEngine
        ${CMAKE_SOURCE_DIR}/TradingStrategies/Common
        ${CMAKE_SOURCE_DIR}/Logger
        ${simdjson_SOURCE_DIR}/include
        ${jsoncpp_SOURCE_DIR}/include
)

# Указание исходных файлов
set(SOURCE_FILES
        BinanceScalping.cpp
        Arbitrage.cpp
        Bellman_Ford.cpp
        ../Common/Common.cpp
        ../../Logger/Logger.cpp
        ../../TradingEngine/OrderManager.cpp
)

# Создание исполняемого файла
add_executable(run_arbitrage ${SOURCE_FILES})

# Линковка библиотек
target_link_libraries(run_arbitrage
        PRIVATE Boost::json
        PRIVATE OpenSSL::SSL
        PRIVATE OpenSSL::Crypto
        PRIVATE simdjson
        PRIVATE jsoncpp_lib
        CURL::libcurl
)
