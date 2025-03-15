#pragma once

#include <vector>
#include "../../../Logger/include/CSVLogger.h"
#include "../../../Logger/include/Logger.h"
#include "../../Common/include/TradingStrategy.h"

class MeanReverseStrategy : public TradingStrategy {
private:
    auto calculate_fee(double amount) -> double;

    auto should_buy(const std::vector<double> &prices, CSVLogger &csv_logger) -> bool override;

    auto should_sell(const std::vector<double> &prices, double entry_price, CSVLogger &csv_logger) -> bool override;

public:
    MeanReverseStrategy() : TradingStrategy({
                                                5, 10, 30.0,
                                                29.0
                                            }, 1000.0, false, 0.0, 0.0, "", "", "") {
    };

    MeanReverseStrategy(const TradingParams &trading_params, double balance_, bool position_open_, double entry_price_,
                        double asset_quantity_, const std::string &key, const std::string &secret,
                        const std::string &symbol) : TradingStrategy(trading_params, balance_, position_open_,
                                                                     entry_price_,
                                                                     asset_quantity_, key, secret, symbol) {
    }

    auto execute(const std::vector<double> &prices, CSVLogger &csv_logger) -> double override;

    auto wrapper_execute(size_t window_size, const std::vector<double> &prices,
                         CSVLogger &logger) -> std::pair<double, double>;
};
