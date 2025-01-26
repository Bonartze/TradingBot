cmake_minimum_required(VERSION 3.16)
project(MyTradingBot)

set(CMAKE_CXX_STANDARD 17)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.87 REQUIRED COMPONENTS json)

if (NOT Boost_JSON_FOUND)
    message(FATAL_ERROR "boost_json not found. Ensure Boost version is 1.75 or newer and properly installed.")
endif ()

find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

include(FetchContent)
FetchContent_Declare(
        simdjson
        GIT_REPOSITORY https://github.com/simdjson/simdjson.git
        GIT_SHALLOW TRUE)
set(SIMDJSON_JUST_LIBRARY ON CACHE INTERNAL "")
set(SIMDJSON_BUILD_STATIC ON CACHE INTERNAL "")
FetchContent_MakeAvailable(simdjson)

FetchContent_Declare(
        jsoncpp
        GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
        GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(jsoncpp)

include_directories(
        ${CMAKE_SOURCE_DIR}/TradingEngine
        ${CMAKE_SOURCE_DIR}/TradingStrategies/Common
        ${CMAKE_SOURCE_DIR}/Logger
        ${simdjson_SOURCE_DIR}/include
        ${jsoncpp_SOURCE_DIR}/include
)

set(SOURCE_FILES
        LiveBinanceScalping.cpp
        Arbitrage.cpp
        Bellman_Ford.cpp
        ../Common/Common.cpp
        ../../Logger/Logger.cpp
        ../../TradingEngine/OrderManager.cpp
        ../Common/BinanceScalping.cpp
        ../Scalping/LiveBinanceScalpingCurrency.cpp
        ../Scalping/LiveBinanceScalpingCurrency.h
        ../include/strategies_api.h
        ../MeanReversionStrategy/include/MeanReverseStr.h
        ../MeanReversionStrategy/src/MeanReverseStr.cpp
)

add_executable(run_arbitrage ${SOURCE_FILES})

target_link_libraries(run_arbitrage
        PRIVATE Boost::json
        PRIVATE OpenSSL::SSL
        PRIVATE OpenSSL::Crypto
        PRIVATE simdjson
        PRIVATE jsoncpp
        CURL::libcurl
)