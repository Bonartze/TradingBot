cmake_minimum_required(VERSION 3.16)

cmake_policy(SET CMP0048 NEW)

project(TradingStrategiesLibrary VERSION 1.0.1 DESCRIPTION "Shared library with strategies API")

set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/../third_party/boost")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/stage/lib")

if (NOT EXISTS "${BOOST_LIBRARYDIR}/libboost_json.a")
    message(STATUS "Boost не найден в third_party. Начинаем сборку...")

    execute_process(COMMAND ./bootstrap.sh
            WORKING_DIRECTORY ${BOOST_ROOT})

    execute_process(COMMAND ./b2 headers
            WORKING_DIRECTORY ${BOOST_ROOT})

    execute_process(COMMAND ./b2 install --prefix=${BOOST_ROOT}
            WORKING_DIRECTORY ${BOOST_ROOT})
endif ()

include(FetchContent)

FetchContent_Declare(
        jsoncpp
        GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
        GIT_TAG master
)
FetchContent_MakeAvailable(jsoncpp)

set_target_properties(jsoncpp_static PROPERTIES POSITION_INDEPENDENT_CODE ON)


FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
        simdjson
        GIT_REPOSITORY https://github.com/simdjson/simdjson.git
        GIT_TAG master
)
FetchContent_MakeAvailable(simdjson)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(StrategiesAPI SHARED
        ../Logger/src/Logger.cpp
        ../TradingEngine/src/BinanceOrderManager.cpp
        Common/src/BinanceScalping.cpp
        Common/src/Common.cpp
        Common/src/TradingMethods.cpp
        Common/src/TradingStrategy.cpp
        IntraExchangeArbitration/src/Arbitrage.cpp
        IntraExchangeArbitration/src/Bellman_Ford.cpp
        IntraExchangeArbitration/src/LiveBinanceScalping.cpp
        Scalping/src/LiveBinanceScalpingCurrency.cpp
        Scalping/src/ScalpingStr.cpp
)

set_target_properties(StrategiesAPI PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(StrategiesAPI PROPERTIES PUBLIC_HEADER include/strategies_api.h)

target_include_directories(StrategiesAPI PRIVATE
        ${BOOST_INCLUDEDIR}
        ${jsoncpp_SOURCE_DIR}/include
        ${nlohmann_json_SOURCE_DIR}/include
        ${simdjson_SOURCE_DIR}/include
)

target_link_directories(StrategiesAPI PRIVATE ${BOOST_LIBRARYDIR})

target_link_libraries(StrategiesAPI PRIVATE
        nlohmann_json::nlohmann_json
        jsoncpp_static
        simdjson
        ${BOOST_LIBRARYDIR}/libboost_json.a
        ${BOOST_LIBRARYDIR}/libboost_system.a
        ${BOOST_LIBRARYDIR}/libboost_container.a
)

include(GNUInstallDirs)

install(TARGETS StrategiesAPI LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

configure_file(StrategiesAPI.pc.in StrategiesAPI.pc @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/StrategiesAPI.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
