cmake_minimum_required(VERSION 3.16)
project(TestApp)

set(CMAKE_CXX_STANDARD 17)

# Указываем пути к заголовочным файлам и библиотекам, если они установлены в стандартные каталоги
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(TestApp main.cpp ../Logger/src/CSVLogger.cpp)
target_link_libraries(TestApp PRIVATE StrategiesAPI)
