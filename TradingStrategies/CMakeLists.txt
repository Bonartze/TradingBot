cmake_minimum_required(VERSION 3.16)

project(TradingStrategiesLibrary VERSION 1.0.1 DESCRIPTION "Shared library with strategies API")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BOOST_ROOT "/usr/local")
set(BOOST_INCLUDEDIR "/usr/local/include")
set(BOOST_LIBRARYDIR "/usr/local/lib")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)

# Получение simdjson
FetchContent_Declare(
        simdjson
        GIT_REPOSITORY https://github.com/simdjson/simdjson.git
        GIT_SHALLOW TRUE
)
set(SIMDJSON_JUST_LIBRARY ON CACHE INTERNAL "")
set(SIMDJSON_BUILD_STATIC ON CACHE INTERNAL "")
FetchContent_MakeAvailable(simdjson)

# Получение jsoncpp
FetchContent_Declare(
        jsoncpp
        GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
        GIT_TAG master
)
FetchContent_MakeAvailable(jsoncpp)

# Получение nlohmann_json
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

find_package(Boost 1.75 REQUIRED COMPONENTS json)

# Подключаем OpenSSL и CURL
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

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
        PRIVATE ${simdjson_SOURCE_DIR}/include
        PRIVATE ${Boost_INCLUDE_DIRS}
        PRIVATE ${jsoncpp_SOURCE_DIR}/include
)

find_package(Eigen3 3.3 REQUIRED NO_MODULE)

target_link_libraries(StrategiesAPI PRIVATE
        nlohmann_json::nlohmann_json
        ${Boost_LIBRARIES}
        Eigen3::Eigen
        simdjson
        OpenSSL::SSL
        OpenSSL::Crypto
        CURL::libcurl
        # Если target для jsoncpp создаётся (например, jsoncpp_lib), его тоже нужно добавить:
        jsoncpp
)

include(GNUInstallDirs)

install(TARGETS StrategiesAPI
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

configure_file(StrategiesAPI.pc.in StrategiesAPI.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/StrategiesAPI.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
