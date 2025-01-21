#pragma once
#include <vector>
#include <fstream>

#include "../Common/Common.h"

constexpr double RSI_OVERSOLD_THRESHOLD = 75.0;
constexpr double RSI_OVERBOUGHT_THRESHOLD = 25.0;

constexpr double PRICE_ABOVE_SMA_THRESHOLD = 0.99;
constexpr double SMA_SHORT_LONG_RATIO_THRESHOLD = 1.0001;

std::ofstream logs_output("/home/ivan/scalping.log"); // переделать потом на другие логировани


struct ScalpingParams {
    size_t sma_short;
    size_t sma_long;
    double rsi_value;
    double rsi_prev;
};


class ScalpingStr {
private:
    double balance;
    bool position_open;
    double entry_price;
    double asset_quantity;


    static auto should_buy(const std::vector<double> &, const ScalpingParams &) -> bool;

    static auto should_sell(const std::vector<double> &, const ScalpingParams &, double) -> bool;

public:
    ScalpingStr(): balance(1000.0), position_open(false), entry_price(0.0), asset_quantity(0.0) {
    };

    static auto loadCandles(const std::string &) -> std::vector<Candle>;

    auto execute(const std::vector<double> &, ScalpingParams) -> void;;
};
