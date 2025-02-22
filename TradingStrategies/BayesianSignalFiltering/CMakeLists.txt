cmake_minimum_required(VERSION 3.14)

project(MeanRevCheck)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(bayes ./src/BayesianSignalFiltering.cpp
                        ../Common/src/TradingMethods.cpp)

