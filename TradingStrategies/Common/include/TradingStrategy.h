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


    virtual auto should_buy(const std::vector<double> &, const TradingParams &, CSVLogger &) -> bool;

    static auto extract_prices(const std::vector<Candle> &) -> std::vector<double>;

    virtual auto should_sell(const std::vector<double> &, const TradingParams &, double, CSVLogger &) -> bool;

    virtual auto execute(const std::vector<double> &, TradingParams, CSVLogger &) -> double;

    virtual auto execute() -> void = 0;

public:
    TradingStrategy(const TradingParams &trading_params, double balance, bool position_open, double entry_price,
                    double asset_quantity);

    virtual ~TradingStrategy();
};
