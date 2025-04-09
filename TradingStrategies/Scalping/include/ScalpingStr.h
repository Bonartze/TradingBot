#pragma once
#include <vector>

#include "../../../Logger/include/CSVLogger.h"
#include "../../Common/include/Common.h"
#include "../../Common/include/TradingStrategy.h"


constexpr double RSI_OVERSOLD_THRESHOLD = 75.0;
constexpr double RSI_OVERBOUGHT_THRESHOLD = 25.0;

constexpr double PRICE_ABOVE_SMA_THRESHOLD = 0.9999;
constexpr double SMA_SHORT_LONG_RATIO_THRESHOLD = 1.0001;

constexpr double SMA_SHORT = 5.0;
constexpr double SMA_LONG = 10.0;
constexpr double RSI_VALUE = 30.0;
constexpr double RSI_PREV = 29.0;
constexpr double BALANCE = 1000.0;

class ScalpingStr : public TradingStrategy {
public:
    ScalpingStr(): TradingStrategy({
                                       SMA_SHORT, SMA_LONG, RSI_VALUE,
                                       RSI_PREV
                                   }, {BALANCE, false, 0.0, 0.0, false}, "", "", "") {
    };

    ScalpingStr(const TradingParams &trading_params, double balance_, bool position_open_, double entry_price_,
                double asset_quantity_, const std::string &key, const std::string &secret,
                const std::string &symbol, bool is_backtesting = true) : TradingStrategy(trading_params, {
            balance_, position_open_, entry_price_,
            asset_quantity_, is_backtesting
        }, symbol, key, secret) {
    }

    auto set_parameters(const std::vector<double> &) -> void;

    auto should_buy(const std::vector<double> &prices,
                    CSVLogger &csv_logger) -> bool override;

    auto should_sell(const std::vector<double> &prices,
                     double entry_price, CSVLogger &csv_logger) -> bool override;

    auto execute() -> void;

    auto execute(const std::vector<double> &prices, CSVLogger &csv_logger) -> double override;

    auto wrapper_execute(size_t window_size, const std::vector<double> &prices,
                         CSVLogger &logger) -> std::pair<double, double>;
};
