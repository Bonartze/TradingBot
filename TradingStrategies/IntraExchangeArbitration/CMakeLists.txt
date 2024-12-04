cmake_minimum_required(VERSION 3.16)
project(MyTradingBot)

set(CMAKE_CXX_STANDARD 17)

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

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)

FetchContent_MakeAvailable(json)

FetchContent_Declare(
        jsoncpp
        GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
        GIT_TAG master
)

FetchContent_MakeAvailable(jsoncpp)

find_package(OpenSSL REQUIRED)

find_package(CURL REQUIRED)


set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS json)

include_directories(
        ${CMAKE_SOURCE_DIR}/TradingEngine
        ${CMAKE_SOURCE_DIR}/TradingStrategies/Common
        ${CMAKE_SOURCE_DIR}/Logger
)

set(SOURCE_FILES
        BinanceScalping.cpp
        Arbitrage.cpp
        Bellman_Ford.cpp
        ../Common/Common.cpp
        ../../Logger/Logger.cpp
        ../../TradingEngine/OrderManager.cpp
)

add_executable(run_arbitrage ${SOURCE_FILES})

target_link_libraries(run_arbitrage
        PRIVATE Boost::json
        PRIVATE OpenSSL::SSL
        PRIVATE OpenSSL::Crypto
        PRIVATE simdjson
        PRIVATE jsoncpp_lib
        CURL::libcurl
)
