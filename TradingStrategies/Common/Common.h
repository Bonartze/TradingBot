#pragma once

#include <unordered_set>

#include <string>

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

