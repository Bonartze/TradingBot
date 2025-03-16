#include "../include/MeanReverseStr.h"
#include "../../Common/include/TradingMethods.h"
#include "../../../Logger/include/CSVLogger.h"
#include <algorithm>
#include <chrono>
#include <thread>

constexpr size_t SHORT_WINDOW_SIZE = 20;
constexpr size_t LONG_WINDOW_SIZE = 30;

auto MeanReverseStrategy::calculate_fee(double amount) -> double {
    return std::max(amount * 0.01, 0.01);
}

auto MeanReverseStrategy::set_parameters(const std::vector<double> &params) -> void {
    if (params.size() < 4) {
        return;
    }
    trading_params = {
        params[0], params[1],
        params[2], params[3]
    };
}


bool MeanReverseStrategy::should_buy(const std::vector<double> &prices, CSVLogger &csv_logger) {
    double ema_short = TradingMethods::ema(prices, trading_params.sma_short);
    double ema_long = TradingMethods::ema(prices, trading_params.sma_long);
    double current_price = prices.back();

    bool buy_signal = ema_short < ema_long && current_price < ema_long;
    std::cerr << "Params: " << buy_signal << " " << ema_short << " " << ema_long << " " << current_price << std::endl;
    Logger(LogLevel::DEBUG) << "Buy check: EMA Short = " << ema_short
            << ", EMA Long = " << ema_long
            << ", Current Price = " << current_price
            << ", Signal: " << buy_signal;
    return buy_signal;
}

bool MeanReverseStrategy::should_sell(const std::vector<double> &prices, double entry_price,
                                      CSVLogger &csv_logger) {
    double ema_short = TradingMethods::ema(prices, trading_params.sma_short);
    double ema_long = TradingMethods::ema(prices, trading_params.sma_long);
    double current_price = prices.back();

    bool sell_signal = ema_short > ema_long && current_price > ema_long;
    Logger(LogLevel::DEBUG) << "Sell check: EMA Short = " << ema_short
            << ", EMA Long = " << ema_long
            << ", Current Price = " << current_price
            << ", Signal: " << sell_signal;
    return sell_signal;
}

auto MeanReverseStrategy::execute(const std::vector<double> &prices, CSVLogger &csv_logger) -> double {
    const double current_price = prices.back();
    double profit = 0.0;


    std::string symbol = "BTCUSDT";

    if (!position_open && should_buy(prices, csv_logger)) {
        const double max_quantity = (balance - calculate_fee(balance)) / current_price;
        if (balance >= max_quantity * current_price + calculate_fee(max_quantity)) {
            asset_quantity = max_quantity;
            entry_price = current_price;
            balance -= asset_quantity * entry_price + calculate_fee(asset_quantity);
            position_open = true;
            std::cerr << "In buy\n";
            Logger(LogLevel::INFO) << "BUY: Price = " << entry_price
                    << ", Quantity = " << asset_quantity
                    << ", Balance = " << balance;
            csv_logger.logRow({
                "BUY", std::to_string(entry_price), std::to_string(asset_quantity),
                std::to_string(balance)
            });


            std::string buy_order_id = std::to_string(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                ).count()
            );

            if (binanceOrderManager && !is_backtesting) {
                binanceOrderManager->place_order(symbol, "BUY", "LIMIT", "GTC",
                                                 asset_quantity, entry_price, buy_order_id,
                                                 0.0, 0.0, 5000);
            } else {
                Logger(LogLevel::ERROR) << "Order manager not initialized for BUY.";
            }
        } else {
            Logger(LogLevel::WARNING) << "Insufficient balance. Current balance: " << balance;
        }
    } else if (position_open && should_sell(prices, entry_price, csv_logger)) {
        const double exit_price = current_price;
        profit = asset_quantity * (exit_price - entry_price) - calculate_fee(asset_quantity);
        balance += asset_quantity * exit_price - calculate_fee(asset_quantity);
        double sell_quantity = asset_quantity;
        asset_quantity = 0.0;
        position_open = false;
        std::cerr << "In sell\n";
        Logger(LogLevel::INFO) << "SELL: Price = " << exit_price
                << ", Profit = " << profit
                << ", Balance = " << balance;
        csv_logger.logRow({
            "SELL", std::to_string(exit_price), "0.0",
            std::to_string(balance), std::to_string(profit)
        });


        std::string sell_order_id = std::to_string(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count()
        );

        if (binanceOrderManager && !is_backtesting) {
            binanceOrderManager->place_order(symbol, "SELL", "LIMIT", "GTC",
                                             sell_quantity, exit_price, sell_order_id,
                                             0.0, 0.0, 5000);
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

auto MeanReverseStrategy::wrapper_execute(size_t window_size, const std::vector<double> &prices,
                                          CSVLogger &csv_logger) -> std::pair<double, double> {
    double total_profit = 0.0;
    size_t trades_count = 0;

    for (size_t i = window_size; i <= prices.size(); i += window_size) {
        std::vector<double> price_segment(prices.begin() + i - window_size, prices.begin() + i);
        total_profit += execute(price_segment, csv_logger);
        trades_count++;
    }

    csv_logger.logRow({"FINAL PROFIT", std::to_string(total_profit), std::to_string(trades_count)});

    return {total_profit, trades_count};
}
