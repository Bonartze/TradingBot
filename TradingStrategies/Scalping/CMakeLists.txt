cmake_minimum_required(VERSION 3.16)
project(MyTradingBot)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
        simdjson
        GIT_REPOSITORY https://github.com/simdjson/simdjson.git
        GIT_SHALLOW TRUE)

set(SIMDJSON_BUILD_STATIC ON)
FetchContent_MakeAvailable(simdjson)

include_directories(
        ${CMAKE_SOURCE_DIR}/TradingEngine
        ${CMAKE_SOURCE_DIR}/TradingStrategies/Common
        ${CMAKE_SOURCE_DIR}/Logger
        ${simdjson_SOURCE_DIR}/include
        ${jsoncpp_SOURCE_DIR}/include
)

set(SOURCE_FILES
        ScalpingStr.cpp
        ../Common/TradingMethods.cpp
        ../Common/TradingMethods.h
        ../../Logger/Logger.cpp
        ../../Backtesting/scalping_backtesting.cpp
        ../../Backtesting/scalping_backtesting.h
        ../Common/BinanceScalping.cpp
)

add_executable(run_scalping ${SOURCE_FILES})

target_link_libraries(run_scalping PRIVATE simdjson)
