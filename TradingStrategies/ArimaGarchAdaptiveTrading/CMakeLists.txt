cmake_minimum_required(VERSION 3.6)

project(example_project)

find_package(Eigen3 3.3 REQUIRED)

set(sources src/ArimaGarchAdaptive.cpp include/ArimaGarchAdaptive.h)

add_executable(example ${sources})

target_link_libraries(example)
