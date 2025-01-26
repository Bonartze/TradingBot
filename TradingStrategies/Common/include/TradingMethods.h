#pragma once
#include <vector>
#include <iostream>

class TradingMethods {
public:
    static auto sma(const std::vector<double> &, size_t) -> double;

    static auto rsi(const std::vector<double> &, int) -> double;

    static auto ema(const std::vector<double> &, size_t) -> double;
};
