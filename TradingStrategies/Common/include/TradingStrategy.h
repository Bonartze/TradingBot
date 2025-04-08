#pragma once
#include "../../../Logger/include/CSVLogger.h"
#include "../../Common/include/Common.h"
#include <memory>
#include "../../../TradingEngine/include/BinanceOrderManager.h"

struct TradeValues {
    double balance;
    bool position_open;
    double entry_price;
    double asset_quantity;
    bool is_backtesting;
};


class TradingStrategy {
protected:
    //NOLINTBEGIN
    bool is_backtesting;
    std::unique_ptr<BinanceOrderManager> binanceOrderManager;
    std::string symbol;
    TradingParams trading_params;
    double balance;
    bool position_open;
    double entry_price;
    double asset_quantity;

    //NOLINTEND
    virtual auto should_buy(const std::vector<double> &, CSVLogger &) -> bool = 0;

    virtual auto should_sell(const std::vector<double> &, double, CSVLogger &) -> bool = 0;

    virtual auto execute(const std::vector<double> &, CSVLogger &) -> double = 0;

public:
    static auto extract_prices(const std::vector<Candle> &) -> std::vector<double>;

    TradingStrategy(const TradingParams &trading_params,
                    const TradeValues &trade_values,
                    const std::string &symbol,
                    const std::string &key,
                    const std::string &secret);

    TradingStrategy(const TradingStrategy &) = delete;

    auto operator=(const TradingStrategy &) -> TradingStrategy & = delete;

    TradingStrategy(TradingStrategy &&) noexcept = default;

    auto operator=(TradingStrategy &&) noexcept -> TradingStrategy & = default;

    virtual ~TradingStrategy() = default;
};
