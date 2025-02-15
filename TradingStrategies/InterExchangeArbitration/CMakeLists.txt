cmake_minimum_required(VERSION 3.14)

project(Arbitrage)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_NO_SYSTEM_PATHS OFF)

set(BOOST_ROOT "/usr/lib/x86_64-linux-gnu")
set(BOOST_INCLUDEDIR "/usr/include")
set(BOOST_LIBRARYDIR "/usr/lib/x86_64-linux-gnu")


include(FetchContent)
find_package(OpenSSL REQUIRED)

FetchContent_Declare(
        simdjson
        GIT_REPOSITORY https://github.com/simdjson/simdjson.git
        GIT_TAG master
)
FetchContent_MakeAvailable(simdjson)

set(SOURCE_FILES
        ../Common/src/BinanceScalping.cpp
        src/OKXAPI.cpp
        src/BinanceAPI.cpp
        src/KrakenAPI.cpp
        src/ExchangeAPI.cpp
)

add_executable(int_ar ${SOURCE_FILES})

target_link_libraries(int_ar PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
        simdjson
)

target_include_directories(int_ar PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${simdjson_SOURCE_DIR}/include
        ${OpenSSL_INCLUDE_DIR}
)
