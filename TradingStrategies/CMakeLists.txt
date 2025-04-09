cmake_minimum_required(VERSION 3.16)
project(TradingStrategiesLibrary VERSION 1.0.1 DESCRIPTION "Shared library with strategies API")

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.28")
    cmake_policy(SET CMP0144 NEW)
endif ()
if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.29")
    cmake_policy(SET CMP0167 NEW)
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Указываем корневые пути к Boost, если нужно (в случае нестандартного расположения)
# set(BOOST_ROOT "/usr")
# set(BOOST_INCLUDEDIR "/usr/include")
# set(BOOST_LIBRARYDIR "/usr/lib")

# Поиск пакетов через apt-установленные dev-пакеты
find_package(Boost 1.75 REQUIRED COMPONENTS json)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(nlohmann_json REQUIRED) # если установлен пакет nlohmann-json3-dev
find_package(jsoncpp REQUIRED)       # возможно, потребуется указать корректный поиск для jsoncpp

# Для simdjson может понадобиться найти пакет, если такой есть в репозитории
find_package(simdjson REQUIRED)      # Если пакет называется libsimdjson-dev

# Если какой-либо пакет не находится, можно попробовать установить его:
# apt install nlohmann-json3-dev libjsoncpp-dev libsimdjson-dev

add_library(StrategiesAPI SHARED
        ../Logger/src/Logger.cpp
        ../TradingEngine/src/AbstractOrderManager.cpp
        ../TradingEngine/src/BinanceOrderManager.cpp
        Common/src/BinanceScalping.cpp
        Common/src/Common.cpp
        Common/src/TradingMethods.cpp
        Common/src/TradingStrategy.cpp
        IntraExchangeArbitration/src/Arbitrage.cpp
        IntraExchangeArbitration/src/Bellman_Ford.cpp
        IntraExchangeArbitration/src/LiveBinanceScalping.cpp
        Scalping/src/ScalpingStr.cpp
        Scalping/src/LiveBinanceScalpingCurrency.cpp
        InterExchangeArbitration/src/ExchangeAPI.cpp
        InterExchangeArbitration/src/BinanceAPI.cpp
        InterExchangeArbitration/src/OKXAPI.cpp
        InterExchangeArbitration/src/KrakenAPI.cpp
        MeanReversionStrategy/src/MeanReverseStr.cpp
        BayesianSignalFiltering/src/BayesianSignalFiltering.cpp
        ../StaticModels/src/ArimaModel.cpp
        ../StaticModels/src/GarchModel.cpp
        ArimaGarchAdaptiveTrading/src/ArimaGarchAdaptive.cpp
        include/strategies_api.cpp
)

set_target_properties(StrategiesAPI PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(StrategiesAPI PROPERTIES PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/strategies_api.h")

target_include_directories(StrategiesAPI
        PRIVATE .
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(StrategiesAPI PRIVATE
        nlohmann_json::nlohmann_json   # если найдётся соответствующий таргет
        ${Boost_LIBRARIES}
        Eigen3::Eigen
        simdjson::simdjson             # если пакет и таргет найдены
        OpenSSL::SSL
        OpenSSL::Crypto
        CURL::libcurl
        jsoncpp_lib                  # название таргета для jsoncpp может отличаться, проверьте документацию
)

include(GNUInstallDirs)

install(TARGETS StrategiesAPI
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

configure_file(StrategiesAPI.pc.in StrategiesAPI.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/StrategiesAPI.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
