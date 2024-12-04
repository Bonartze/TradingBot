include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)
FetchContent_Declare(
        jsoncpp
        GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
        GIT_TAG master
)

FetchContent_MakeAvailable(jsoncpp)

find_package(Boost 1.86.0 REQUIRED)


add_library(OrderManager TradingServer.cpp OrderManager.cpp ../Logger/Logger.cpp)

target_link_libraries(OrderManager PRIVATE nlohmann_json::nlohmann_json jsoncpp_lib)
