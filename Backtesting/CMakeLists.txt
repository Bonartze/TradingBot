cmake_minimum_required(VERSION 3.14)

project(ScalpingBacktesting)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch Google Test
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
FetchContent_MakeAvailable(googletest)

# Fetch jsoncpp
FetchContent_Declare(
        jsoncpp
        GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
        GIT_TAG master
)
FetchContent_MakeAvailable(jsoncpp)

# Fetch simdjson
FetchContent_Declare(
        simdjson
        GIT_REPOSITORY https://github.com/simdjson/simdjson.git
        GIT_TAG master
)
set(SIMDJSON_JUST_LIBRARY ON CACHE INTERNAL "")
FetchContent_MakeAvailable(simdjson)

# Find Boost
find_package(Boost REQUIRED COMPONENTS system filesystem)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Add executable
add_executable(backtesting
        ./src/scalping_backtesting.cpp
        ../TradingStrategies/Scalping/src/ScalpingStr.cpp
        ./src/run_backtesting.cpp
        ../Logger/src/Logger.cpp
        ../TradingStrategies/Common/src/TradingMethods.cpp
        ../Logger/src/CSVLogger.cpp
        ../TradingStrategies/Common/src/BinanceScalping.cpp
        ../TradingStrategies/Common/src/Common.cpp
)

# Link libraries
target_link_libraries(backtesting PRIVATE
        gtest
        gtest_main
        jsoncpp_object
        simdjson
        Boost::system
        Boost::filesystem
        OpenSSL::SSL
        OpenSSL::Crypto
)

# Include directories
target_include_directories(backtesting PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${simdjson_SOURCE_DIR}/include
)

enable_testing()

add_test(NAME BacktestingTest COMMAND backtesting)
