include(FetchContent)

FetchContent_Declare(
        simdjson
        GIT_REPOSITORY https://github.com/simdjson/simdjson.git
        GIT_TAG v0.4.7
        GIT_SHALLOW TRUE)

set(SIMDJSON_JUST_LIBRARY ON CACHE INTERNAL "")
set(SIMDJSON_BUILD_STATIC ON CACHE INTERNAL "")


FetchContent_MakeAvailable(simdjson)


find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

link_directories(${OPENSSL_LIBRARIES})
set(Boost_USE_STATIC_LIBS ON)

find_package(Boost REQUIRED COMPONENTS json)

include_directories(${BOOST_INCLUDE_DIRS})

add_executable(run_arbitrage BinanceScalping.cpp Bellman_Ford.cpp Arbitrage.cpp Common.cpp)
target_link_libraries(run_arbitrage PRIVATE Boost::json OpenSSL::SSL OpenSSL::Crypto simdjson)

