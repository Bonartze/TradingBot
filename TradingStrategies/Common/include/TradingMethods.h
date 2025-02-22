#pragma once
#include <vector>
#include <iostream>

class TradingMethods {
public:
    static auto sma(const std::vector<double> &, size_t) -> double;

    static auto rsi(const std::vector<double> &, int) -> double;

    static auto ema(const std::vector<double> &, size_t) -> double;

    static auto rsi_m(const std::vector<double> &prices, int period) -> std::vector<double>;

    static auto sma_m(const std::vector<double> &prices, int period) -> std::vector<double>;


    static auto bollinger_bands(const std::vector<double> &prices, int period,
                                double numStdDev) -> std::tuple<double, double, double>;
};
