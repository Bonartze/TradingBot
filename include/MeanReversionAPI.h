#pragma once

#include <vector>
#include "StrategiesAPIExport.h"
#include "../TradingStrategies/Common/include/TradingStrategy.h"
#include "../Logger/include/CSVLogger.h"

class STRATEGIES_API MeanReverseStrategy : public TradingStrategy {
private:
    auto calculate_fee(double amount) -> double;
    auto should_buy(const std::vector<double> &prices, CSVLogger &csv_logger) -> bool override;
    auto should_sell(const std::vector<double> &prices, double entry_price, CSVLogger &csv_logger) -> bool override;

public:
  	MeanReverseStrategy();
    MeanReverseStrategy(const TradingParams &trading_params, double balance, bool position_open, double entry_price, double asset_quantity);
    auto execute(const std::vector<double> &prices, CSVLogger &csv_logger) -> double override;
};
