#pragma once
#include <vector>

#include "../../../Logger/include/CSVLogger.h"
#include "../../Common/include/Common.h"
#include "../../Common/include/TradingStrategy.h"


constexpr double RSI_OVERSOLD_THRESHOLD = 75.0;
constexpr double RSI_OVERBOUGHT_THRESHOLD = 25.0;

constexpr double PRICE_ABOVE_SMA_THRESHOLD = 0.9999;
constexpr double SMA_SHORT_LONG_RATIO_THRESHOLD = 1.0001;


class ScalpingStr : public TradingStrategy {
public:
    ScalpingStr(): TradingStrategy({
                                       5, 10, 30.0,
                                       29.0
                                   }, 1000.0, false, 0.0, 0.0) {
    };

    ScalpingStr(const TradingParams &trading_params, double balance_, bool position_open_, double entry_price_,
                double asset_quantity_) : TradingStrategy(trading_params, balance_, position_open_, entry_price_,
                                                          asset_quantity_) {
    }

    auto should_buy(const std::vector<double> &prices,
                    CSVLogger &csv_logger) -> bool;

    auto should_sell(const std::vector<double> &prices,
                     double entry_price, CSVLogger &csv_logger) -> bool;

    auto execute() -> void;

    auto execute(const std::vector<double> &, CSVLogger &) -> double;

    auto wrapper_execute(size_t window_size, const std::vector<double> &prices,
                         CSVLogger &logger) -> std::pair<double, double>;
};
