cmake_minimum_required(VERSION 3.16)
project(MyTradingBot)

set(CMAKE_CXX_STANDARD 17)
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
)

add_executable(run_scalping ${SOURCE_FILES})
