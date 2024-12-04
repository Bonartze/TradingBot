cmake_minimum_required(VERSION 3.14)

project(Arbitrage)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_STATIC_LIBS ON)

include(FetchContent)

# Fetch simdjson
FetchContent_Declare(
        simdjson
        GIT_REPOSITORY https://github.com/simdjson/simdjson.git
        GIT_TAG v0.4.7
        GIT_SHALLOW TRUE
)

set(SIMDJSON_JUST_LIBRARY ON CACHE INTERNAL "")
set(SIMDJSON_BUILD_STATIC ON CACHE INTERNAL "")

FetchContent_MakeAvailable(simdjson)

# Fetch nlohmann/json
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# Fetch jsoncpp
FetchContent_Declare(
        jsoncpp
        GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
        GIT_TAG master
)

FetchContent_MakeAvailable(jsoncpp)

# Find Boost components (excluding json)
find_package(Boost REQUIRED COMPONENTS beast asio container)

# Find OpenSSL and CURL
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

include_directories(
        ${Boost_INCLUDE_DIRS}
)

set(SOURCE_FILES
        FetchData.cpp
)

add_executable(run_arbitrage ${SOURCE_FILES})

target_link_libraries(run_arbitrage
        PRIVATE Boost::beast
        PRIVATE Boost::asio
        PRIVATE Boost::container
        PRIVATE OpenSSL::SSL
        PRIVATE OpenSSL::Crypto
        PRIVATE simdjson
        PRIVATE jsoncpp_lib
        CURL::libcurl
)

message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
