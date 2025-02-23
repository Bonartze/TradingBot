cmake_minimum_required(VERSION 3.16)

cmake_policy(SET CMP0048 NEW)

project(TradingStrategiesLibrary VERSION 1.0.1 DESCRIPTION "Shared library with strategies API")

include(FetchContent)

# Подключаем simdjson
FetchContent_Declare(
        simdjson
        GIT_REPOSITORY https://github.com/simdjson/simdjson.git
        GIT_SHALLOW TRUE)
set(SIMDJSON_JUST_LIBRARY ON CACHE INTERNAL "")
set(SIMDJSON_BUILD_STATIC ON CACHE INTERNAL "")
FetchContent_MakeAvailable(simdjson)

# Подключаем jsoncpp
FetchContent_Declare(
        jsoncpp
        GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
        GIT_TAG master
)
FetchContent_MakeAvailable(jsoncpp)

# Подключаем nlohmann_json
FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

# Находим Boost с поддержкой json
find_package(Boost 1.75 REQUIRED COMPONENTS json)

add_library(StrategiesAPI SHARED
        ../Logger/src/Logger.cpp
        ../TradingEngine/src/BinanceOrderManager.cpp
        Common/src/BinanceScalping.cpp
        Common/src/Common.cpp Common/src/TradingMethods.cpp
        Common/src/TradingStrategy.cpp
        IntraExchangeArbitration/src/Arbitrage.cpp
        IntraExchangeArbitration/src/Bellman_Ford.cpp
        IntraExchangeArbitration/src/LiveBinanceScalping.cpp
        Scalping/src/LiveBinanceScalpingCurrency.cpp
        Scalping/src/ScalpingStr.cpp)

set_target_properties(StrategiesAPI PROPERTIES VERSION ${PROJECT_VERSION})

set_target_properties(StrategiesAPI PROPERTIES PUBLIC_HEADER include/strategies_api.h)

target_include_directories(StrategiesAPI PRIVATE
        ${simdjson_SOURCE_DIR}/include
        ${Boost_INCLUDE_DIRS}  # Подключаем Boost
        ${jsoncpp_SOURCE_DIR}/include
        ${nlohmann_json_SOURCE_DIR}/include
)

target_link_libraries(StrategiesAPI PRIVATE
        nlohmann_json::nlohmann_json
        Boost::json  # Линкуем Boost.JSON
)

include(GNUInstallDirs)

install(TARGETS StrategiesAPI LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

configure_file(StrategiesAPI.pc.in StrategiesAPI.pc @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/StrategiesAPI.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
