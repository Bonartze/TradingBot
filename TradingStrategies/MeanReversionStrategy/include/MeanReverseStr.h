#pragma once

#include <vector>
#include "../../../Logger/include/CSVLogger.h"
#include "../../../Logger/include/Logger.h"
#include "../../Common/include/TradingStrategy.h"

class MeanReverseStrategy : public TradingStrategy {
private:
    auto calculate_fee(double amount) -> double;

    bool should_buy(const std::vector<double> &prices, const TradingParams &scalping_params,
                    CSVLogger &csv_logger);

    bool should_sell(const std::vector<double> &prices, const TradingParams &scalping_params,
                     const double &entry_price, CSVLogger &csv_logger);

public:
    auto execute(const std::vector<double> &prices, const TradingParams &trading_params,
                 CSVLogger &csv_logger) -> double;
    auto execute() -> void override;
};
