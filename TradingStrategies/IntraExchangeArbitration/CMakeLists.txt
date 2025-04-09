cmake_minimum_required(VERSION 3.14)
project(MeanRevCheck)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(jsoncpp REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(simdjson REQUIRED)
find_package(CURL REQUIRED)

add_executable(aa
        ./src/Arbitrage.cpp
        ./src/Bellman_Ford.cpp
        ./src/LiveBinanceScalping.cpp
        ../../Logger/src/Logger.cpp
        ../Common/src/Common.cpp
        ../../TradingEngine/src/BinanceOrderManager.cpp
        ../Common/src/BinanceScalping.cpp
        ../Common/src/TradingMethods.cpp
        ../Scalping/src/ScalpingStr.cpp
        ../../Logger/src/CSVLogger.cpp
        ../Scalping/src/LiveBinanceScalpingCurrency.cpp
        ../../TradingEngine/src/AbstractOrderManager.cpp
)


target_link_libraries(aa PRIVATE
        JsonCpp::JsonCpp
        OpenSSL::SSL
        OpenSSL::Crypto
        simdjson
        CURL::libcurl
        ${OPENSSL_LIBRARIES}
)


target_include_directories(aa PRIVATE
        ../../Logger/include
        ../Common/include
        ../../TradingEngine/include
        ${OPENSSL_INCLUDE_DIR}
)