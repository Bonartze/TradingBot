#pragma once
#include <vector>

struct BollingerBandsParams {
    int period;
    double num_std_dev;
};

class TradingMethods {
public:
    static auto sma(const std::vector<double> &, size_t) -> double;

    static auto rsi(const std::vector<double> &, size_t) -> double;

    static auto ema(const std::vector<double> &, size_t) -> double;

    static auto rsi_m(const std::vector<double> &prices, int period) -> std::vector<double>;

    static auto sma_m(const std::vector<double> &prices, int period) -> std::vector<double>;


    static auto bollinger_bands(const std::vector<double> &prices,
                                const BollingerBandsParams &params) -> std::tuple<double, double, double>;
};
