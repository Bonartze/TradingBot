cmake_minimum_required(VERSION 3.16)

cmake_policy(SET CMP0048 NEW)

project(TradingStrategiesLibrary VERSION 1.0.1 DESCRIPTION "Shared library with strategies API")

set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/../third_party")
set(BOOST_ROOT "${THIRD_PARTY_DIR}/boost_1_87_0")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/stage/lib")

set(JSONCPP_ROOT "${THIRD_PARTY_DIR}/jsoncpp_install")
set(JSONCPP_INCLUDEDIR "${JSONCPP_ROOT}/include")
set(JSONCPP_LIBRARYDIR "${JSONCPP_ROOT}/lib")

set(NLOHMANN_JSON_HPP "${THIRD_PARTY_DIR}/nlohmann_json/json.hpp") # Если это просто файл

set(SIMDJSON_ROOT "${THIRD_PARTY_DIR}/simdjson_install")
set(SIMDJSON_INCLUDEDIR "${SIMDJSON_ROOT}/include")
set(SIMDJSON_LIBRARYDIR "${SIMDJSON_ROOT}/lib")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "BOOST_ROOT: ${BOOST_ROOT}")
message(STATUS "BOOST_INCLUDEDIR: ${BOOST_INCLUDEDIR}")
message(STATUS "BOOST_LIBRARYDIR: ${BOOST_LIBRARYDIR}")
message(STATUS "JSONCPP_ROOT: ${JSONCPP_ROOT}")
message(STATUS "JSONCPP_INCLUDEDIR: ${JSONCPP_INCLUDEDIR}")
message(STATUS "JSONCPP_LIBRARYDIR: ${JSONCPP_LIBRARYDIR}")
message(STATUS "NLOHMANN_JSON_HPP: ${NLOHMANN_JSON_HPP}")
message(STATUS "SIMDJSON_ROOT: ${SIMDJSON_ROOT}")
message(STATUS "SIMDJSON_INCLUDEDIR: ${SIMDJSON_INCLUDEDIR}")
message(STATUS "SIMDJSON_LIBRARYDIR: ${SIMDJSON_LIBRARYDIR}")

if (NOT EXISTS "${BOOST_LIBRARYDIR}/libboost_json.a")
    message(FATAL_ERROR "Boost не найден в third_party. Сначала собери его вручную.")
endif ()

if (NOT EXISTS "${JSONCPP_LIBRARYDIR}/libjsoncpp.a")
    message(FATAL_ERROR "JsonCpp не найден в third_party/jsoncpp_install. Собери его вручную.")
endif ()

if (NOT EXISTS "${SIMDJSON_LIBRARYDIR}/libsimdjson.a")
    message(FATAL_ERROR "Simdjson не найден в third_party/simdjson_install. Собери его вручную.")
endif ()

find_package(Boost REQUIRED COMPONENTS json system container)

find_package(OpenSSL REQUIRED)

find_package(CURL REQUIRED)

add_library(StrategiesAPI SHARED
        ../Logger/src/Logger.cpp
        ../Logger/src/CSVLogger.cpp
        ../TradingEngine/src/BinanceOrderManager.cpp
        ../TradingEngine/src/AbstractOrderManager.cpp
        Common/src/BinanceScalping.cpp
        MeanReversionStrategy/src/MeanReverseStr.cpp
        Common/src/Common.cpp
        Common/src/TradingMethods.cpp
        Common/src/TradingStrategy.cpp
        IntraExchangeArbitration/src/Arbitrage.cpp
        IntraExchangeArbitration/src/Bellman_Ford.cpp
        IntraExchangeArbitration/src/LiveBinanceScalping.cpp
        Scalping/src/LiveBinanceScalpingCurrency.cpp
        Scalping/src/ScalpingStr.cpp
        Scalping/include/ScalpingStr.h
)

target_include_directories(StrategiesAPI PRIVATE
        ${BOOST_INCLUDEDIR}
        ${JSONCPP_INCLUDEDIR}
        ${SIMDJSON_INCLUDEDIR}
        ${CMAKE_SOURCE_DIR}/../TradingEngine/include
)

add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE ${THIRD_PARTY_DIR}/nlohmann_json)

target_link_directories(StrategiesAPI PRIVATE
        ${BOOST_LIBRARYDIR}
        ${JSONCPP_LIBRARYDIR}
        ${SIMDJSON_LIBRARYDIR}
)

target_link_libraries(StrategiesAPI PRIVATE
        ${BOOST_LIBRARYDIR}/libboost_json.a
        ${BOOST_LIBRARYDIR}/libboost_system.a
        ${BOOST_LIBRARYDIR}/libboost_container.a
        ${JSONCPP_LIBRARYDIR}/libjsoncpp.a
        ${SIMDJSON_LIBRARYDIR}/libsimdjson.a
        OpenSSL::SSL
        OpenSSL::Crypto
        CURL::libcurl
        nlohmann_json
)

set_target_properties(StrategiesAPI PROPERTIES PUBLIC_HEADER
        ../include/ArbitrageAPI.h
        ../include/ExchangeAPI.h
        ../include/MeanReversionAPI.h
        ../include/ScalpingAPI.h
        ../include/StrategiesAPIExport.h
)

include(GNUInstallDirs)

install(TARGETS StrategiesAPI LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

configure_file(StrategiesAPI.pc.in StrategiesAPI.pc @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/StrategiesAPI.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
