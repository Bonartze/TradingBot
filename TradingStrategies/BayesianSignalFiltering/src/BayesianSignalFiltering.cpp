#include "../include/BayesianSignalFiltering.h"
#include "../../Common/include/TradingMethods.h"
constexpr int LIMIT_MIN_SIZE = 20;
constexpr int PERIOD = 14;
constexpr double NUM_STD_DEV = 2.0;
constexpr int RECV_WINDOW = 5000;

auto BayesianSignalFiltering::should_buy(const std::vector<double> &prices, CSVLogger &csv_logger) -> bool {
    if (prices.size() < LIMIT_MIN_SIZE) {
        return false;
    }
    std::vector<double> rsi_values = TradingMethods::rsi_m(prices, PERIOD);
    auto [lower_band, middle_band, upper_band] = TradingMethods::bollinger_bands(prices, {LIMIT_MIN_SIZE, NUM_STD_DEV});

    const double current_price = prices.back();
    const double current_rsi = rsi_values.back();
    const double current_lower_band = lower_band;

    csv_logger.logRow({
        std::to_string(current_price), std::to_string(current_rsi),
        std::to_string(current_lower_band)
    });

    const double p_a = historical_up_probability(prices, csv_logger);
    const double p_b_given_a = signal_probability_given_up(prices, rsi_values, lower_band);
    const double p_b = signal_frequency(prices, rsi_values, lower_band);
    const double p_a_given_b = (p_b_given_a * p_a) / p_b;

    return p_a_given_b > buyThreshold;
}

auto BayesianSignalFiltering::should_sell(const std::vector<double> &prices, [[maybe_unused]] double entry_price,
                                          CSVLogger &csv_logger) -> bool {
    if (prices.size() < LIMIT_MIN_SIZE) {
        return false;
    }
    std::vector<double> rsi_values = TradingMethods::rsi_m(prices, PERIOD);
    auto [lower_band, middle_band, upper_band] = TradingMethods::bollinger_bands(prices, {LIMIT_MIN_SIZE, NUM_STD_DEV});

    const double current_price = prices.back();
    const double current_rsi = rsi_values.back();
    const double current_upper_band = upper_band;

    csv_logger.logRow({
        std::to_string(current_price), std::to_string(current_rsi),
        std::to_string(current_upper_band)
    });

    const double p_a = historical_down_probability(prices, csv_logger);
    const double p_b_given_a = signal_probability_given_down(prices, rsi_values, upper_band);
    const double p_b = signal_frequency(prices, rsi_values, upper_band);
    const double p_a_given_b = (p_b_given_a * p_a) / p_b;


    return p_a_given_b > sellThreshold;
}

auto BayesianSignalFiltering::historical_up_probability(const std::vector<double> &prices,
                                                        [[maybe_unused]] CSVLogger &csv_logger) -> double {
    int up_count = 0;
    for (size_t i = 1; i < prices.size(); ++i) {
        if (prices[i] > prices[i - 1]) {
            up_count++;
        }
    }
    return static_cast<double>(up_count) / static_cast<double>(prices.size() - 1);
}

auto BayesianSignalFiltering::historical_down_probability(const std::vector<double> &prices,
                                                          [[maybe_unused]] CSVLogger &csv_logger) -> double {
    int down_count = 0;
    for (size_t i = 1; i < prices.size(); ++i) {
        if (prices[i] < prices[i - 1]) {
            down_count++;
        }
    }
    return static_cast<double>(down_count) / static_cast<double>(prices.size() - 1);
}

auto BayesianSignalFiltering::signal_probability_given_up(const std::vector<double> &prices,
                                                          const std::vector<double> &rsi_values,
                                                          double lower_band) const -> double {
    int signal_and_up = 0;
    int up_count = 0;
    for (size_t i = 1; i < prices.size(); ++i) {
        if (prices[i] > prices[i - 1]) {
            up_count++;

            if (rsi_values[i] < rsiThreshold && prices[i] < lower_band) {
                signal_and_up++;
            }
        }
    }
    return up_count > 0 ? static_cast<double>(signal_and_up) / up_count : 0.0;
}

auto BayesianSignalFiltering::signal_probability_given_down(const std::vector<double> &prices,
                                                            const std::vector<double> &rsi_values,
                                                            double upper_band) const -> double {
    int signal_and_down = 0;
    int down_count = 0;
    for (size_t i = 1; i < prices.size(); ++i) {
        if (prices[i] < prices[i - 1]) {
            down_count++;
            if (rsi_values[i - 1] > rsiThreshold && prices[i - 1] > upper_band) {
                signal_and_down++;
            }
        }
    }
    return down_count > 0 ? static_cast<double>(signal_and_down) / down_count : 0.0;
}

auto BayesianSignalFiltering::signal_frequency(const std::vector<double> &prices,
                                               const std::vector<double> &rsi_values,
                                               double lower_band) const -> double {
    int signal_count = 0;
    for (size_t i = 0; i < prices.size(); ++i) {
        if (rsi_values[i] < rsiThreshold && prices[i] < lower_band) {
            signal_count++;
        }
    }
    return static_cast<double>(signal_count) / static_cast<double>(prices.size());
}

auto BayesianSignalFiltering::execute(const std::vector<double> &prices, CSVLogger &csv_logger) -> double {
    if (prices.size() < LIMIT_MIN_SIZE) {
        return 0.0;
    }

    const double current_price = prices.back();
    double profit = 0.0;

    csv_logger.logRow({"Executing strategy", std::to_string(current_price)});
    Logger(LogLevel::DEBUG) << "Position open: " << position_open
            << " Should buy: " << should_buy(prices, csv_logger)
            << " Should sell: " << should_sell(prices, entry_price, csv_logger) << '\n';

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
            if (!is_backtesting) {
                binanceOrderManager->place_order(symbol, "BUY", "LIMIT", "GTC",
                                                 asset_quantity, entry_price, buy_order_id, 0.0, 0.0, RECV_WINDOW);
            }
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
        const std::string sell_order_id = std::to_string(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count()
        );
        if (!is_backtesting) {
            binanceOrderManager->place_order(symbol, "SELL", "LIMIT", "GTC",
                                             sell_quantity, exit_price, sell_order_id, 0.0, 0.0, RECV_WINDOW);
        }
        asset_quantity = 0.0;
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

void BayesianSignalFiltering::set_parameters(const std::vector<double> &params) {
    if (params.size() < 2) {
        throw std::invalid_argument("Not enough parameters provided");
    }
    buyThreshold = params[0];
    sellThreshold = params[0];
    rsiThreshold = params[1];
}


auto BayesianSignalFiltering::wrapper_execute(size_t window_size, const std::vector<double> &prices,
                                              CSVLogger &csv_logger) -> std::pair<double, double> {
    double total_profit = 0.0;
    size_t trades_count = 0;

    for (size_t i = window_size; i <= prices.size(); i += window_size) {
        const std::vector<double> price_segment(
            prices.begin() + static_cast<std::ptrdiff_t>(i) - static_cast<std::ptrdiff_t>(window_size),
            prices.begin() + static_cast<std::ptrdiff_t>(i)
        );
        total_profit += execute(price_segment, csv_logger);
        trades_count++;
    }

    if (position_open) {
        const double final_price = prices.back();
        const double profit = asset_quantity * (final_price - entry_price);
        balance += asset_quantity * final_price;
        total_profit += profit;
        asset_quantity = 0.0;
        position_open = false;
        trades_count++;

        csv_logger.logRow({
            "FINAL SELL", std::to_string(final_price), "0.0",
            std::to_string(balance), std::to_string(profit)
        });
    }

    csv_logger.logRow({"FINAL PROFIT", std::to_string(total_profit), std::to_string(trades_count)});

    return {total_profit, trades_count};
}
