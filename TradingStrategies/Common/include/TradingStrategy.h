#pragma once
#include "../../../Logger/include/CSVLogger.h"
#include "../../Common/include/Common.h"

class TradingStrategy {
protected:
    TradingParams trading_params;
    double balance;
    bool position_open;
    double entry_price;
    double asset_quantity;

    virtual auto should_buy(const std::vector<double> &, CSVLogger &) -> bool = 0;

    virtual auto should_sell(const std::vector<double> &, double, CSVLogger &) -> bool = 0;

    virtual auto execute(const std::vector<double> &, CSVLogger &) -> double = 0;

public:
    static auto extract_prices(const std::vector<Candle> &) -> std::vector<double>;

    TradingStrategy(const TradingParams &trading_params, double balance, bool position_open_, double entry_price_,
                    double asset_quantity_);

    virtual ~TradingStrategy() = default;
};
