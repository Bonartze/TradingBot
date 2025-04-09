#pragma once

#include <vector>
#include "../../../Logger/include/CSVLogger.h"
#include "../../Common/include/TradingStrategy.h"

constexpr double MEAN_REV_BALANCE = 1000.0;
constexpr size_t MEAN_REV_SMA_SHORT = 5;
constexpr size_t MEAN_REV_SMA_LONG = 10;
constexpr double MEAN_REV_RSI_OVERSOLD = 30;
constexpr double MEAN_REV_RSI_OVERBOUGHT = 29;

class MeanReverseStrategy : public TradingStrategy {
    static auto calculate_fee(double amount) -> double;

    auto should_buy(const std::vector<double> &prices, CSVLogger &csv_logger) -> bool override;

    auto should_sell(const std::vector<double> &prices, double entry_price, CSVLogger &csv_logger) -> bool override;

public:
    MeanReverseStrategy()
        : TradingStrategy(
            TradingParams{
                MEAN_REV_SMA_SHORT,
                MEAN_REV_SMA_LONG,
                MEAN_REV_RSI_OVERSOLD,
                MEAN_REV_RSI_OVERBOUGHT
            },
            TradeValues{
                MEAN_REV_BALANCE,
                false,
                0.0,
                0.0,
                false
            },
            "",
            "",
            ""
        ) {
    };

    MeanReverseStrategy(const TradingParams &trading_params,
                        double balance_,
                        bool position_open_,
                        double entry_price_,
                        double asset_quantity_,
                        const std::string &key,
                        const std::string &secret,
                        const std::string &symbol,
                        bool is_backtesting = true)
        : TradingStrategy(
            trading_params,
            TradeValues{
                balance_,
                position_open_,
                entry_price_,
                asset_quantity_,
                is_backtesting
            },
            symbol,
            key,
            secret
        ) {
    }

    auto set_parameters(const std::vector<double> &newParams) -> void;

    auto execute(const std::vector<double> &prices, CSVLogger &csv_logger) -> double override;

    auto wrapper_execute(size_t window_size, const std::vector<double> &prices,
                         CSVLogger &logger) -> std::pair<double, double>;
};
