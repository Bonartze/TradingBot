cmake_minimum_required(VERSION 3.25.1)
project(TradingBot VERSION 1.0)

#add_subdirectory(DatabaseWrapper)
add_subdirectory(Autotests)
add_subdirectory(TradingEngine)
add_subdirectory(TradingStrategies/IntraExchangeArbitration)
