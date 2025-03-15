#pragma once
#include "../../../Logger/include/CSVLogger.h"
#include "../../Common/include/Common.h"
#include <memory>
#include "../../../TradingEngine/include/BinanceOrderManager.h"

class TradingStrategy {
protected:
    bool is_backtesting;
    std::unique_ptr<BinanceOrderManager> binanceOrderManager;
    std::string symbol;
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
                    double asset_quantity_, const std::string &, const std::string &, const std::string &, bool is_backtesting_ = true);

    virtual ~TradingStrategy() = default;
};
