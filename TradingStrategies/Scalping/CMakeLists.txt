cmake_minimum_required(VERSION 3.16)
project(MyTradingBot)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# Подключение simdjson
FetchContent_Declare(
        simdjson
        GIT_REPOSITORY https://github.com/simdjson/simdjson.git
        GIT_SHALLOW TRUE)

set(SIMDJSON_BUILD_STATIC ON)
FetchContent_MakeAvailable(simdjson)

# Подключение Boost
find_package(Boost REQUIRED COMPONENTS system filesystem regex)
if (NOT Boost_FOUND)
    message(FATAL_ERROR "Boost libraries not found!")
endif ()

# Подключение OpenSSL
find_package(OpenSSL REQUIRED)
if (NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL not found!")
endif ()

# Подключение libcurl
find_package(CURL REQUIRED)
if (NOT CURL_FOUND)
    message(FATAL_ERROR "CURL library not found!")
endif ()

# Указание директорий для заголовков
include_directories(
        ${CMAKE_SOURCE_DIR}/TradingEngine
        ${CMAKE_SOURCE_DIR}/TradingStrategies/Common
        ${CMAKE_SOURCE_DIR}/Logger
        ${simdjson_SOURCE_DIR}/include
        ${Boost_INCLUDE_DIRS}
        ${OPENSSL_INCLUDE_DIR}
)

# Указание файлов исходного кода
set(SOURCE_FILES
        src/ScalpingStr.cpp
        ../Common/src/TradingMethods.cpp
        ../../Logger/src/Logger.cpp
        ../../Logger/src/CSVLogger.cpp
        ../Common/src/BinanceScalping.cpp
        src/LiveBinanceScalpingCurrency.cpp
        ../Common/src/Common.cpp
        ../Common/src/TradingStrategy.cpp
        ../../Backtesting/src/mean_reverse_backtesting.cpp
        ../../Backtesting/include/mean_reverse_backtesting.h
)

# Создание исполняемого файла
add_executable(run_scalping ${SOURCE_FILES})

# Линковка библиотек
target_link_libraries(run_scalping PRIVATE
        simdjson
        Boost::system
        Boost::filesystem
        Boost::regex
        OpenSSL::SSL
        OpenSSL::Crypto
        CURL::libcurl
)

# Установка дополнительных флагов компиляции
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(run_scalping PRIVATE -Wall -Wextra -Wpedantic)
endif ()

# Указание директории для исполняемых файлов
set_target_properties(run_scalping PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
