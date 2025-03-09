cmake_minimum_required(VERSION 3.6)

project(example_project)

find_package(Eigen3 3.3 REQUIRED)

set(sources src/GarchModel.cpp src/ArimaModel.cpp src/main.cpp ../TradingStrategies/Common/src/Common.cpp ../Logger/src/Logger.cpp)

add_executable(example ${sources})

target_link_libraries(example Eigen3::Eigen)
