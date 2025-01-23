cmake_minimum_required(VERSION 3.14)

project(ScalpingBacktesting)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

message(STATUS "Fetching GTest...")

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(backtesting scalping_backtesting.cpp ../TradingStrategies/Scalping/ScalpingStr.cpp
        run_backtesting.cpp ../Logger/Logger.cpp ../TradingStrategies/Common/TradingMethods.cpp
        ../Logger/CSVLogger.cpp
        ../Logger/CSVLogger.h
        ../TradingStrategies/Common/BinanceScalping.cpp
        ../TradingStrategies/Common/BinanceScalping.h)

target_link_libraries(backtesting PRIVATE gtest gtest_main)

enable_testing()

add_test(NAME BacktestingTest COMMAND backtesting)
