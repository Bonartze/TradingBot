#pragma once

#include <unordered_set>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include "../../../Logger/include/Logger.h"

extern std::unordered_set<std::string> currencies;

extern const char *const host;
extern const char *const target;
extern const char *const port;

struct Candle {
    long timestamp;
    double open;
    double high;
    double low;
    double close;
    double volume;
};

struct TradingParams {
    double sma_short;
    double sma_long;
    double rsi_value;
    double rsi_prev;
};

auto loadCandles(const std::string &filename) -> std::vector<Candle>;
