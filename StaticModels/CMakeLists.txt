cmake_minimum_required(VERSION 3.0)

project(example_project)

find_package(Eigen3 3.3 REQUIRED)

set(sources src/ArimaModel.cpp)

add_executable(example ${sources})

target_link_libraries(example Eigen3::Eigen)
