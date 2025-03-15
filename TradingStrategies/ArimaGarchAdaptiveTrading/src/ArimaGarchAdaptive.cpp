#include "../include/ArimaGarchAdaptive.h"
#include <cmath>
#include <chrono>
#include <thread>


constexpr double HIGH_VOLATILITY_LEVEL = 50.0;
constexpr double LOW_VOLATILITY_LEVEL = 150.0;

auto ArimaGarchAdaptive::should_buy(const std::vector<double> &prices, CSVLogger &csv_logger) -> bool {
    if (prices.empty())
        return false;

    double current_price = prices.back();
    auto arima_pred = arima_model->forecast(1);
    auto garch_pred = garch_model->forecast(arima_model->get_residuals(), 1);

    double arima_forecast = arima_pred[0];
    double volatility = std::sqrt(garch_pred[0]);

    csv_logger.logRow({
        "should_buy", std::to_string(current_price), std::to_string(arima_forecast), std::to_string(volatility)
    });

    if (volatility < LOW_VOLATILITY_LEVEL)
        return arima_forecast > current_price;
    else if (volatility > HIGH_VOLATILITY_LEVEL)
        return arima_forecast < current_price;

    return false;
}

auto ArimaGarchAdaptive::should_sell(const std::vector<double> &prices, double entry_price,
                                     CSVLogger &csv_logger) -> bool {
    if (prices.empty())
        return false;

    double current_price = prices.back();
    auto garch_pred = garch_model->forecast(arima_model->get_residuals(), 1);
    double volatility = std::sqrt(garch_pred[0]);

    csv_logger.logRow({
        "should_sell", std::to_string(current_price), std::to_string(entry_price), std::to_string(volatility)
    });


    if (current_price >= entry_price * 1.02)
        return true;
    if (volatility > 200.0)
        return true;

    return false;
}

auto ArimaGarchAdaptive::execute(const std::vector<double> &prices, CSVLogger &csv_logger) -> double {
    if (prices.size() < 20)
        return 0.0;

    const double current_price = prices.back();
    double profit = 0.0;

    csv_logger.logRow({"Executing strategy", std::to_string(current_price)});
    Logger(LogLevel::DEBUG) << "Position open: " << position_open
            << " | Should buy: " << should_buy(prices, csv_logger)
            << " | Should sell: " << should_sell(prices, entry_price, csv_logger);


    if (!position_open && should_buy(prices, csv_logger)) {
        const double max_quantity = balance / current_price;
        if (max_quantity > 0) {
            asset_quantity = max_quantity;
            entry_price = current_price;
            balance -= asset_quantity * entry_price;
            position_open = true;


            const std::string buy_order_id = std::to_string(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                ).count()
            );


            binanceOrderManager->place_order(symbol, "BUY", "LIMIT", "GTC",
                                             asset_quantity, entry_price, buy_order_id,
                                             0.0, 0.0, 5000);

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
    } else if (position_open && should_sell(prices, entry_price, csv_logger)) {
        const double exit_price = current_price;
        const double sell_quantity = asset_quantity;
        profit = sell_quantity * (exit_price - entry_price);
        balance += sell_quantity * exit_price;
        position_open = false;
        asset_quantity = 0.0;


        const std::string sell_order_id = std::to_string(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count()
        );

        binanceOrderManager->place_order(symbol, "SELL", "LIMIT", "GTC",
                                         sell_quantity, exit_price, sell_order_id,
                                         0.0, 0.0, 5000);

        Logger(LogLevel::INFO) << "Sell at: " << exit_price
                << " | Profit: " << profit
                << " | Total Balance: " << balance;
        csv_logger.logRow({
            "SELL", std::to_string(exit_price), "0.0",
            std::to_string(balance), std::to_string(profit)
        });
    } else {
        Logger(LogLevel::DEBUG) << "No action taken. Position Open: " << position_open
                << " | Current Price: " << current_price;
        csv_logger.logRow({
            "HOLD", std::to_string(current_price), std::to_string(asset_quantity),
            std::to_string(balance)
        });
    }

    return profit;
}

auto ArimaGarchAdaptive::wrapper_execute(size_t window_size, const std::vector<double> &prices,
                                         CSVLogger &csv_logger) -> std::pair<double, double> {
    double total_profit = 0.0;
    size_t trades_count = 0;


    for (size_t i = window_size; i <= prices.size(); i += window_size) {
        std::vector<double> price_segment(prices.begin() + i - window_size, prices.begin() + i);
        total_profit += execute(price_segment, csv_logger);
        trades_count++;
    }


    if (position_open) {
        const double final_price = prices.back();
        double final_profit = asset_quantity * (final_price - entry_price);
        balance += asset_quantity * final_price;
        total_profit += final_profit;
        asset_quantity = 0.0;
        position_open = false;
        trades_count++;

        Logger(LogLevel::INFO) << "Final sell at: " << final_price
                << " | Profit: " << final_profit
                << " | Total Balance: " << balance;
        csv_logger.logRow({
            "FINAL SELL", std::to_string(final_price), "0.0",
            std::to_string(balance), std::to_string(final_profit)
        });
    }

    Logger(LogLevel::INFO) << "Final Profit: " << total_profit
            << " | Total Trades: " << trades_count;
    csv_logger.logRow({
        "FINAL PROFIT", std::to_string(total_profit), std::to_string(trades_count)
    });

    return {total_profit, trades_count};
}
