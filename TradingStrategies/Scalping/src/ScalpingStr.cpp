#include "../include/ScalpingStr.h"
#include "../../../Logger/include/Logger.h"
#include "../../Common/include/TradingMethods.h"
#include <filesystem>
#include <chrono>

constexpr int RECV_WINDOW = 5000;

auto ScalpingStr::should_buy(const std::vector<double> &prices,
                             CSVLogger &csv_logger) -> bool {
    //std::cout <<"Pries sb: "<< prices.size() << std::endl;
    const double sma_short = TradingMethods::sma(prices, static_cast<size_t>(trading_params.sma_short));
    const double sma_long = TradingMethods::sma(prices, static_cast<size_t>(trading_params.sma_long));
    const double current_price = prices.back();

    Logger(LogLevel::DEBUG) << "Buy Check - Current Price: " << current_price
            << ", SMA Short: " << sma_short
            << ", SMA Long: " << sma_long
            << ", RSI: " << trading_params.rsi_value;

    csv_logger.logRow({
        "BUY_CHECK", std::to_string(current_price), std::to_string(sma_short),
        std::to_string(sma_long), std::to_string(trading_params.rsi_value)
    });

    return (current_price > sma_short * PRICE_ABOVE_SMA_THRESHOLD &&
            sma_short > sma_long * SMA_SHORT_LONG_RATIO_THRESHOLD &&
            trading_params.rsi_value < RSI_OVERSOLD_THRESHOLD);
}

auto ScalpingStr::should_sell(const std::vector<double> &prices,
                              double entry_price, CSVLogger &csv_logger) -> bool {
    const double sma_short = TradingMethods::sma(prices, static_cast<size_t>(trading_params.sma_short));
    const double current_price = prices.back();

    Logger(LogLevel::DEBUG) << "Sell Check - Current Price: " << current_price
            << ", SMA Short: " << sma_short
            << ", Entry Price: " << entry_price
            << ", RSI: " << trading_params.rsi_value;

    csv_logger.logRow({
        "SELL_CHECK", std::to_string(current_price), std::to_string(sma_short),
        std::to_string(entry_price), std::to_string(trading_params.rsi_value)
    });

    return ((current_price < sma_short * SMA_SHORT_LONG_RATIO_THRESHOLD &&
             trading_params.rsi_value > RSI_OVERBOUGHT_THRESHOLD &&
             current_price > entry_price) ||
            current_price < entry_price * PRICE_ABOVE_SMA_THRESHOLD);
}

void ScalpingStr::set_parameters(const std::vector<double> &params) {
    if (params.size() < 4) {
        return;
    }
    trading_params = {
        params[0], params[1],
        params[2], params[3]
    };
}


auto ScalpingStr::execute(const std::vector<double> &prices,
                          CSVLogger &csv_logger) -> double {
    const double current_price = prices.back();
    double profit = 0.0;
    //! crutch, change ASAP
    const std::string symbol = "BTCUSDT";

    if (!position_open && should_buy(prices, csv_logger)) {
        const double max_quantity = balance / current_price;
        if (max_quantity > 0) {
            asset_quantity = max_quantity;
            entry_price = current_price;
            balance -= asset_quantity * entry_price;
            position_open = true;

            Logger(LogLevel::INFO) << "BUY: Price = " << entry_price
                    << ", Quantity = " << asset_quantity
                    << ", Remaining Balance = " << balance;
            csv_logger.logRow({
                "BUY", std::to_string(entry_price), std::to_string(asset_quantity),
                std::to_string(balance)
            });


            const std::string buy_order_id = std::to_string(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                ).count()
            );

            if (binanceOrderManager && !is_backtesting) {
                binanceOrderManager->place_order(symbol, "BUY", "LIMIT", "GTC",
                                                 asset_quantity, entry_price, buy_order_id,
                                                 0.0, 0.0, RECV_WINDOW);
            } else {
                Logger(LogLevel::ERROR) << "Order manager not initialized for BUY.";
            }
        } else {
            Logger(LogLevel::WARNING) << "Not enough balance to buy. Current balance: " << balance;
        }
    } else if (position_open && should_sell(prices, entry_price, csv_logger)) {
        const double exit_price = current_price;
        profit = asset_quantity * (exit_price - entry_price);
        balance += asset_quantity * exit_price;

        const double sell_quantity = asset_quantity;
        asset_quantity = 0.0;
        position_open = false;

        Logger(LogLevel::INFO) << "SELL: Price = " << exit_price
                << ", Profit = " << profit
                << ", Total Balance = " << balance;
        csv_logger.logRow({
            "SELL", std::to_string(exit_price), "0.0",
            std::to_string(balance), std::to_string(profit)
        });


        const std::string sell_order_id = std::to_string(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count()
        );
        if (binanceOrderManager && !is_backtesting) {
            binanceOrderManager->place_order(symbol, "SELL", "LIMIT", "GTC",
                                             sell_quantity, exit_price, sell_order_id,
                                             0.0, 0.0, RECV_WINDOW);
        } else {
            Logger(LogLevel::ERROR) << "Order manager not initialized for SELL.";
        }
    } else {
        Logger(LogLevel::DEBUG) << "HOLD: Position Open = " << position_open
                << ", Current Price = " << current_price;
        csv_logger.logRow({
            "HOLD", std::to_string(current_price), std::to_string(asset_quantity),
            std::to_string(balance)
        });
    }

    return profit;
}

auto ScalpingStr::wrapper_execute(size_t window_size, const std::vector<double> &prices,
                                  CSVLogger &logger) -> std::pair<double, double> {
    double total_profit = 0.0;
    size_t trades_count = 0;
    for (size_t i = window_size; i <= prices.size(); i += window_size) {
        auto start = prices.begin() + static_cast<std::ptrdiff_t>(i - window_size);
        auto end = prices.begin() + static_cast<std::ptrdiff_t>(i);
        const std::vector<double> price_segment(start, end);
        total_profit += execute(price_segment, logger);
        trades_count++;
    }

    logger.logRow({"FINAL PROFIT", std::to_string(total_profit)});

    return {total_profit, trades_count};
}
