cmake_minimum_required(VERSION 3.14)

project(ScalpingBacktesting)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
        jsoncpp
        GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
        GIT_TAG master
)
FetchContent_MakeAvailable(jsoncpp)

FetchContent_Declare(
        simdjson
        GIT_REPOSITORY https://github.com/simdjson/simdjson.git
        GIT_TAG master
)
set(SIMDJSON_JUST_LIBRARY ON CACHE INTERNAL "")
FetchContent_MakeAvailable(simdjson)

find_package(Boost REQUIRED COMPONENTS system filesystem)

find_package(OpenSSL REQUIRED)
find_package(Eigen3 REQUIRED)

add_executable(backtesting
        ../TradingStrategies/MeanReversionStrategy/src/MeanReverseStr.cpp
        ../TradingStrategies/Scalping/src/ScalpingStr.cpp
        ./src/run_backtesting.cpp
        ../Logger/src/Logger.cpp
        ../TradingStrategies/Common/src/TradingMethods.cpp
        ../TradingStrategies/ArimaGarchAdaptiveTrading/src/ArimaGarchAdaptive.cpp
        ../StaticModels/src/GarchModel.cpp
        ../StaticModels/src/ArimaModel.cpp
        ../Logger/src/CSVLogger.cpp
        ../TradingStrategies/Common/src/BinanceScalping.cpp
        ../TradingStrategies/Common/src/Common.cpp
        ../TradingStrategies/Common/src/TradingStrategy.cpp
        ../TradingStrategies/BayesianSignalFiltering/src/BayesianSignalFiltering.cpp
#        ./src/scalping_backtesting.cpp
#        ./src/mean_reverse_backtesting.cpp
#        ./src/bayes_filtering_backtesting.cpp
        ./src/arima_garch_adaptive_backtesting.cpp
)

target_link_libraries(backtesting PRIVATE
        gtest
        gtest_main
        jsoncpp_object
        simdjson
        OpenSSL::SSL
        OpenSSL::Crypto
        Eigen3::Eigen
)

target_include_directories(backtesting PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${simdjson_SOURCE_DIR}/include
        ${OpenSSL_INCLUDE_DIR}
)

enable_testing()
add_test(NAME BacktestingTest COMMAND backtesting)
