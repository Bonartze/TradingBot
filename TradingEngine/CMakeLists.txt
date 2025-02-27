cmake_minimum_required(VERSION 3.16)

cmake_policy(SET CMP0048 NEW)

project(TradingStrategiesLibrary VERSION 1.0.1 DESCRIPTION "Shared library with strategies API")

# Указываем пути к сторонним библиотекам
set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/../third_party")
set(BOOST_ROOT "${THIRD_PARTY_DIR}/boost_1_87_0")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/stage/lib")

set(JSONCPP_ROOT "${THIRD_PARTY_DIR}/jsoncpp")
set(JSONCPP_BUILD_DIR "${JSONCPP_ROOT}/build")  # Добавлено для правильной сборки
set(JSONCPP_INSTALL_DIR "${JSONCPP_ROOT}/install")
set(JSONCPP_INCLUDEDIR "${JSONCPP_INSTALL_DIR}/include")
set(JSONCPP_LIBRARYDIR "${JSONCPP_INSTALL_DIR}/lib")

set(NLOHMANN_JSON_INCLUDE_DIR "${THIRD_PARTY_DIR}/nlohmann_json") # Директория, где лежит json.hpp

set(SIMDJSON_ROOT "${THIRD_PARTY_DIR}/simdjson")
set(SIMDJSON_INCLUDEDIR "${SIMDJSON_ROOT}/include")

# Вывод отладочной информации
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "BOOST_ROOT: ${BOOST_ROOT}")
message(STATUS "BOOST_INCLUDEDIR: ${BOOST_INCLUDEDIR}")
message(STATUS "BOOST_LIBRARYDIR: ${BOOST_LIBRARYDIR}")
message(STATUS "JSONCPP_ROOT: ${JSONCPP_ROOT}")
message(STATUS "JSONCPP_INCLUDEDIR: ${JSONCPP_INCLUDEDIR}")
message(STATUS "JSONCPP_LIBRARYDIR: ${JSONCPP_LIBRARYDIR}")
message(STATUS "NLOHMANN_JSON_INCLUDE_DIR: ${NLOHMANN_JSON_INCLUDE_DIR}")
message(STATUS "SIMDJSON_ROOT: ${SIMDJSON_ROOT}")
message(STATUS "SIMDJSON_INCLUDEDIR: ${SIMDJSON_INCLUDEDIR}")

# Проверяем, собран ли Boost
if (NOT EXISTS "${BOOST_LIBRARYDIR}/libboost_json.a")
    message(FATAL_ERROR "Boost не найден в third_party. Сначала собери его вручную.")
endif ()

# Проверяем, собран ли jsoncpp с -fPIC, если нет — пересобираем
if (NOT EXISTS "${JSONCPP_LIBRARYDIR}/libjsoncpp.a")
    message(STATUS "JsonCPP не найден в install/. Начинаем сборку...")
    execute_process(
            COMMAND rm -rf ${JSONCPP_BUILD_DIR} ${JSONCPP_INSTALL_DIR}
            COMMAND mkdir -p ${JSONCPP_BUILD_DIR}
            WORKING_DIRECTORY ${JSONCPP_ROOT}
    )

    execute_process(
            COMMAND cmake .. -DCMAKE_INSTALL_PREFIX=${JSONCPP_INSTALL_DIR} -DCMAKE_POSITION_INDEPENDENT_CODE=ON
            WORKING_DIRECTORY ${JSONCPP_BUILD_DIR}
    )

    execute_process(
            COMMAND make -j$(nproc)
            WORKING_DIRECTORY ${JSONCPP_BUILD_DIR}
    )

    execute_process(
            COMMAND make install
            WORKING_DIRECTORY ${JSONCPP_BUILD_DIR}
    )
endif ()

if (NOT EXISTS "${JSONCPP_LIBRARYDIR}/libjsoncpp.a")
    message(FATAL_ERROR "Ошибка: JsonCPP не был собран или установлен.")
endif ()

# Ищем Boost
find_package(Boost REQUIRED COMPONENTS json system container)

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

# Подключаем заголовочные файлы
target_include_directories(StrategiesAPI PRIVATE
        ${BOOST_INCLUDEDIR}
        ${JSONCPP_INCLUDEDIR}
        ${NLOHMANN_JSON_INCLUDE_DIR}  # У nlohmann нет include/, исправлено
        ${SIMDJSON_INCLUDEDIR}
        ${CMAKE_SOURCE_DIR}/../TradingEngine/include  # Убеждаемся, что TradingEngine видит зависимости
)

# Подключаем библиотеки
target_link_directories(StrategiesAPI PRIVATE
        ${BOOST_LIBRARYDIR}
        ${JSONCPP_LIBRARYDIR}
)

target_link_libraries(StrategiesAPI PRIVATE
        ${BOOST_LIBRARYDIR}/libboost_json.a
        ${BOOST_LIBRARYDIR}/libboost_system.a
        ${BOOST_LIBRARYDIR}/libboost_container.a
        ${JSONCPP_LIBRARYDIR}/libjsoncpp.a
        ${SIMDJSON_ROOT}/libsimdjson.a
)

# Устанавливаем заголовочные файлы
set_target_properties(StrategiesAPI PROPERTIES PUBLIC_HEADER
        ../include/ArbitrageAPI.h
        ../include/ExchangeAPI.h
        ../include/MeanReversionAPI.h
        ../include/ScalpingAPI.h
        ../include/StrategiesAPIExport.h
)

# Устанавливаем библиотеку
include(GNUInstallDirs)

install(TARGETS StrategiesAPI LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

configure_file(StrategiesAPI.pc.in StrategiesAPI.pc @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/StrategiesAPI.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
