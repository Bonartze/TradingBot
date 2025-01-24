#pragma once
#include <vector>
#include <fstream>
#include <optional>

#include "../../../Logger/include/CSVLogger.h"
#include "../../Common/include/Common.h"
constexpr double RSI_OVERSOLD_THRESHOLD = 75.0;
constexpr double RSI_OVERBOUGHT_THRESHOLD = 25.0;

constexpr double PRICE_ABOVE_SMA_THRESHOLD = 0.9999;
constexpr double SMA_SHORT_LONG_RATIO_THRESHOLD = 1.0001;

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


    static auto should_buy(const std::vector<double> &, const ScalpingParams &, CSVLogger &) -> bool;

    static auto should_sell(const std::vector<double> &, const ScalpingParams &, double, CSVLogger &) -> bool;

public:
    ScalpingStr(): balance(1000.0), position_open(false), entry_price(0.0), asset_quantity(0.0) {
    };

    ScalpingStr(double balance_, bool position_opened_, double entry_price_, // for backtesting
                double asset_quantity_) : balance(balance_), position_open(position_opened_), entry_price(entry_price_),
                                          asset_quantity(asset_quantity_) {
    }

    static auto extract_prices(const std::vector<Candle> &) -> std::vector<double>;

    static auto loadCandles(const std::string &) -> std::vector<Candle>;

    auto execute(const std::vector<double> &, ScalpingParams, CSVLogger &) -> double;
};
