#pragma once

#include <vector>
#include "StrategiesAPIExport.h"
#include "../TradingStrategies/Common/include/TradingStrategy.h"
#include "../Logger/include/CSVLogger.h"

class STRATEGIES_API ScalpingStr : public TradingStrategy {
public:
    ScalpingStr();
    ScalpingStr(const TradingParams &trading_params, double balance, bool position_open, double entry_price, double asset_quantity);
    auto should_buy(const std::vector<double> &prices, CSVLogger &csv_logger) -> bool;
    auto should_sell(const std::vector<double> &prices, double entry_price, CSVLogger &csv_logger) -> bool;
    auto execute(const std::vector<double> &prices, CSVLogger &csv_logger) -> double;
};
