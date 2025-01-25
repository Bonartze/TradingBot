#include "../include/ScalpingStr.h"
#include "../../../Logger/include/Logger.h"
#include "../../Common/include/TradingMethods.h"
#include <sstream>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <chrono>

auto ScalpingStr::loadCandles(const std::string &filename) -> std::vector<Candle> {
    std::vector<Candle> candles;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss_(line);
        std::string value;
        Candle candle = {};

        std::getline(ss_, value, ',');
        candle.timestamp = std::stol(value);
        std::getline(ss_, value, ',');
        candle.open = std::stod(value);
        std::getline(ss_, value, ',');
        candle.high = std::stod(value);
        std::getline(ss_, value, ',');
        candle.low = std::stod(value);
        std::getline(ss_, value, ',');
        candle.close = std::stod(value);
        std::getline(ss_, value, ',');
        candle.volume = std::stod(value);

        candles.push_back(candle);
    }

    Logger(LogLevel::INFO) << "Loaded candles: " << candles.size() << " from file: " << filename;
    return candles;
}

auto ScalpingStr::should_buy(const std::vector<double> &prices, const ScalpingParams &scalping_params,
                             CSVLogger &csv_logger) -> bool {
    const double sma_short = TradingMethods::sma(prices, scalping_params.sma_short);
    const double sma_long = TradingMethods::sma(prices, scalping_params.sma_long);
    const double current_price = prices.back();

    Logger(LogLevel::DEBUG) << "Buy Check - Current Price: " << current_price
            << ", SMA Short: " << sma_short
            << ", SMA Long: " << sma_long
            << ", RSI: " << scalping_params.rsi_value;

    csv_logger.logRow({
        "BUY_CHECK", std::to_string(current_price), std::to_string(sma_short),
        std::to_string(sma_long), std::to_string(scalping_params.rsi_value)
    });

    return (current_price > sma_short * PRICE_ABOVE_SMA_THRESHOLD &&
            sma_short > sma_long * SMA_SHORT_LONG_RATIO_THRESHOLD &&
            scalping_params.rsi_value < RSI_OVERSOLD_THRESHOLD);
}

auto ScalpingStr::should_sell(const std::vector<double> &prices, const ScalpingParams &scalping_params,
                              double entry_price, CSVLogger &csv_logger) -> bool {
    const double sma_short = TradingMethods::sma(prices, scalping_params.sma_short);
    const double current_price = prices.back();

    Logger(LogLevel::DEBUG) << "Sell Check - Current Price: " << current_price
            << ", SMA Short: " << sma_short
            << ", Entry Price: " << entry_price
            << ", RSI: " << scalping_params.rsi_value;

    csv_logger.logRow({
        "SELL_CHECK", std::to_string(current_price), std::to_string(sma_short),
        std::to_string(entry_price), std::to_string(scalping_params.rsi_value)
    });

    return ((current_price < sma_short * SMA_SHORT_LONG_RATIO_THRESHOLD &&
             scalping_params.rsi_value > RSI_OVERBOUGHT_THRESHOLD &&
             current_price > entry_price) || // Sold with profit
            current_price < entry_price * PRICE_ABOVE_SMA_THRESHOLD); // Stop loss
}

auto ScalpingStr::execute(const std::vector<double> &prices, ScalpingParams sp_params,
                          CSVLogger &csv_logger) -> double { // implements trades later
    const double current_price = prices.back();
    double profit = 0.0;

    if (!position_open && should_buy(prices, sp_params, csv_logger)) {
        const double max_quantity = balance / current_price;
        if (max_quantity > 0) {
            asset_quantity = max_quantity;
            entry_price = current_price;
            balance -= asset_quantity * entry_price;
            position_open = true;

            Logger(LogLevel::INFO) << "Buy at: " << entry_price
                    << " | Quantity: " << asset_quantity
                    << " | Remaining Balance: " << balance;

            csv_logger.logRow({
                "BUY", std::to_string(entry_price), std::to_string(asset_quantity),
                std::to_string(balance)
            });
        } else {
            Logger(LogLevel::WARNING) << "Not enough balance to buy. Current balance: " << balance;
        }
    } else if (position_open && should_sell(prices, sp_params, entry_price, csv_logger)) {
        const double exit_price = current_price;
        profit = asset_quantity * (exit_price - entry_price);
        balance += asset_quantity * exit_price;
        asset_quantity = 0.0;
        position_open = false;

        Logger(LogLevel::INFO) << "Sell at: " << exit_price
                << " | Profit: " << profit
                << " | Total Balance: " << balance;

        csv_logger.logRow({
            "SELL", std::to_string(exit_price), "0.0",
            std::to_string(balance), std::to_string(profit)
        });
    } else {
        Logger(LogLevel::DEBUG) << "No action taken. Position Open: " << position_open
                << ", Current Price: " << current_price;

        csv_logger.logRow({
            "HOLD", std::to_string(current_price), std::to_string(asset_quantity),
            std::to_string(balance)
        });
    }

    return profit;
}


auto ScalpingStr::extract_prices(const std::vector<Candle> &candles) -> std::vector<double> {
    std::vector<double> prices(candles.size());
    std::transform(candles.begin(), candles.end(), prices.begin(),
                   [](const Candle &candle) { return candle.close; });
    Logger(LogLevel::DEBUG) << "Extracted prices: " << prices.size();
    return prices;
}
