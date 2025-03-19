cmake_minimum_required(VERSION 3.10)
project(DataCollectorExample)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск библиотеки libcurl
find_package(CURL REQUIRED)

# Получаем nlohmann_json через FetchContent (или используйте пакет, если установлен)
include(FetchContent)
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

# Добавляем исходники
add_executable(DataCollectorExample src/DataCollector.cpp)

# Подключаем include директории
target_include_directories(DataCollectorExample PRIVATE ${CURL_INCLUDE_DIRS})

# Линкуем с libcurl и nlohmann_json
target_link_libraries(DataCollectorExample PRIVATE ${CURL_LIBRARIES} nlohmann_json::nlohmann_json)
