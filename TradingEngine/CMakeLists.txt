cmake_minimum_required(VERSION 3.15)

# Устанавливаем имя проекта и язык программирования
project(TradingBot LANGUAGES CXX)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подключаем FetchContent для зависимости
include(FetchContent)

# Подключаем библиотеку nlohmann/json
FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

# Подключаем библиотеку jsoncpp
FetchContent_Declare(
        jsoncpp
        GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
        GIT_TAG master
)
FetchContent_MakeAvailable(jsoncpp)

# Найти Boost (если требуется)
find_package(Boost 1.86.0 REQUIRED)

# Укажите пути к исходным файлам
set(SOURCES
        TradingServer.cpp
        OrderManager.cpp
        ../Logger/Logger.cpp
)

# Создаем библиотеку OrderManager
add_library(OrderManager ${SOURCES})

# Подключаем библиотеки
target_link_libraries(OrderManager
        PRIVATE
        nlohmann_json::nlohmann_json
        jsoncpp_lib
)

# Установите дополнительные include директории, если это необходимо
target_include_directories(OrderManager
        PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Включаем строгую компиляцию
target_compile_options(OrderManager PRIVATE -Wall -Wextra -Werror)

# Сообщение о завершении
message(STATUS "CMake configuration is complete.")
