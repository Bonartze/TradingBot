#include "ScalpingStr.h"
#include "../../Logger/Logger.h"
#include "../Common/TradingMethods.h"
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

    Logger(LogLevel::INFO, logs_output) << "Loaded candles: " << candles.size() << " from file: " << filename;
    return candles;
}

auto ScalpingStr::should_buy(const std::vector<double> &prices, const ScalpingParams &scalping_params) -> bool {
    const double sma_short = TradingMethods::sma(prices, scalping_params.sma_short);
    const double sma_long = TradingMethods::sma(prices, scalping_params.sma_long);
    const double current_price = prices.back();

    Logger(LogLevel::DEBUG, logs_output) << "Buy Check - Current Price: " << current_price
            << ", SMA Short: " << sma_short << ", SMA Long: " << sma_long
            << ", RSI: " << scalping_params.rsi_value;

    return (current_price > sma_short * PRICE_ABOVE_SMA_THRESHOLD &&
            sma_short > sma_long * SMA_SHORT_LONG_RATIO_THRESHOLD &&
            scalping_params.rsi_value < RSI_OVERSOLD_THRESHOLD);
}

auto ScalpingStr::should_sell(const std::vector<double> &prices, const ScalpingParams &scalping_params,
                              double entry_price) -> bool {
    const double sma_short = TradingMethods::sma(prices, scalping_params.sma_short);
    const double current_price = prices.back();

    Logger(LogLevel::DEBUG, logs_output) << "Sell Check - Current Price: " << current_price
            << ", SMA Short: " << sma_short
            << ", Entry Price: " << entry_price
            << ", RSI: " << scalping_params.rsi_value;

    return ((current_price < sma_short * SMA_SHORT_LONG_RATIO_THRESHOLD &&
             scalping_params.rsi_value > RSI_OVERBOUGHT_THRESHOLD &&
             current_price > entry_price) || // Sold with profit
            current_price < entry_price * PRICE_ABOVE_SMA_THRESHOLD); // Stop loss
}

auto ScalpingStr::execute(const std::vector<double> &prices, ScalpingParams sp_params) -> void {
    const double current_price = prices.back();

    if (!position_open && should_buy(prices, sp_params)) {
        const double max_quantity = balance / current_price;
        if (max_quantity > 0) {
            asset_quantity = max_quantity;
            entry_price = current_price;
            balance -= asset_quantity * entry_price;
            position_open = true;

            Logger(LogLevel::INFO, logs_output) << "Buy at: " << entry_price
                    << " | Quantity: " << asset_quantity
                    << " | Remaining Balance: " << balance;
        } else {
            Logger(LogLevel::WARNING, logs_output) << "Not enough balance to buy. Current balance: " << balance;
        }
    } else if (position_open && should_sell(prices, sp_params, entry_price)) {
        const double exit_price = current_price;
        const double profit = asset_quantity * (exit_price - entry_price);
        balance += asset_quantity * exit_price;
        asset_quantity = 0.0;
        position_open = false;

        Logger(LogLevel::INFO, logs_output) << "Sell at: " << exit_price
                << " | Profit: " << profit
                << " | Total Balance: " << balance;
    } else {
        Logger(LogLevel::DEBUG, logs_output) << "No action taken. Position Open: " << position_open
                << ", Current Price: " << current_price;
    }
}

auto extract_prices(const std::vector<Candle> &candles) -> std::vector<double> {
    std::vector<double> prices(candles.size());
    std::transform(candles.begin(), candles.end(), prices.begin(),
                   [](const Candle &candle) { return candle.close; });
    Logger(LogLevel::DEBUG, logs_output) << "Extracted prices: " << prices.size();
    return prices;
}

int main() {
    ScalpingStr scalp;
    const std::string data_dir = "/home/ivan/CLionProjects/TradingBot/Backtesting/Data_extracted/5m/BTCUSDT/2024/";

    std::vector<std::filesystem::path> files;
    for (const auto &entry: std::filesystem::directory_iterator(data_dir)) {
        if (entry.path().extension() == ".csv") {
            files.push_back(entry.path());
        }
    }

    std::sort(files.begin(), files.end(), [](const std::filesystem::path &a, const std::filesystem::path &b) {
        return a.filename().string() < b.filename().string();
    });

    Logger(LogLevel::INFO, logs_output) << "Starting backtesting. Total files: " << files.size();

    for (const auto &file: files) {
        Logger(LogLevel::INFO, logs_output) << "Processing file: " << file;

        auto start_time = std::chrono::high_resolution_clock::now();
        auto candles = ScalpingStr::loadCandles(file);
        auto prices = extract_prices(candles);

        const size_t window_size = 100;
        size_t total_trades = 0;
        double total_profit = 0.0;

        for (size_t i = window_size; i <= prices.size(); i += window_size) {
            std::vector<double> price_segment(prices.begin(), prices.begin() + i);
            scalp.execute(price_segment, {5, 20, 30.0, 29.0});
            total_trades++;
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

        Logger(LogLevel::INFO, logs_output) << "File Processed: " << file
                << " | Total Trades: " << total_trades
                << " | Duration: " << duration << " ms";
    }

    Logger(LogLevel::INFO, logs_output) << "Backtesting completed.";
    return 0;
}
