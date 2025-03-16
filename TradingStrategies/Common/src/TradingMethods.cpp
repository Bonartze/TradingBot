#include "../include/TradingMethods.h"
#include <numeric>
#include <iostream>
#include <cmath>
#include <tuple>

auto TradingMethods::sma(const std::vector<double> &prices, size_t period) -> double {
    if (prices.empty()) {
        throw std::invalid_argument("No prices provided");
    }
    size_t effective_period = std::min(prices.size(), period);
    std::cout << "Prices sma: " << prices.size() << " Period: " << effective_period << std::endl;
    const double sum = std::accumulate(prices.end() - effective_period, prices.end(), 0.0);
    return sum / static_cast<double>(effective_period);
}

auto TradingMethods::rsi(const std::vector<double> &prices, size_t period) -> double {
    size_t effective_period = std::min(prices.size(), static_cast<size_t>(period));
    if (effective_period < 2) {
        throw std::invalid_argument("Not enough prices provided for RSI calculation");
    }
    double gain = 0.0;
    double loss = 0.0;
    for (size_t i = 1; i < effective_period; ++i) {
        const double change = prices[i] - prices[i - 1];
        if (change > 0) {
            gain += change;
        } else {
            loss -= change;
        }
    }
    gain /= effective_period;
    loss /= effective_period;
    if (loss == 0) {
        return 100.0;
    }
    const double rs_ = gain / loss;
    return 100.0 - (100.0 / (1.0 + rs_));
}

auto TradingMethods::rsi_m(const std::vector<double> &prices, int period) -> std::vector<double> {
    if (prices.size() < period) {
        throw std::invalid_argument("Not enough prices provided");
    }

    std::vector<double> rsi_values;
    double gain = 0.0, loss = 0.0;

    for (int i = 1; i < period; ++i) {
        const double change = prices[i] - prices[i - 1];
        if (change > 0) {
            gain += change;
        } else {
            loss -= change;
        }
    }

    gain /= period;
    loss /= period;

    for (size_t i = period; i < prices.size(); ++i) {
        const double change = prices[i] - prices[i - 1];

        if (change > 0) {
            gain = (gain * (period - 1) + change) / period;
            loss = (loss * (period - 1)) / period;
        } else {
            gain = (gain * (period - 1)) / period;
            loss = (loss * (period - 1) - change) / period;
        }

        const double rs_ = (loss == 0) ? 100.0 : gain / loss;
        const double rsi_value = 100.0 - (100.0 / (1.0 + rs_));
        rsi_values.push_back(rsi_value);
    }

    return rsi_values;
}


auto TradingMethods::ema(const std::vector<double> &prices, size_t window_size) -> double {
    if (prices.size() < window_size) {
        throw std::invalid_argument("Not enough data to calculate EMA.");
    }

    const double koef = 2.0 / (static_cast<double>(window_size) + 1);

    double ema_prev = std::accumulate(prices.begin(), prices.begin() + static_cast<double>(window_size), 0.0) /
                      static_cast<double>(window_size);

    for (size_t i = window_size; i < prices.size(); ++i) {
        ema_prev = (prices[i] * koef) + (ema_prev * (1 - koef));
    }

    return ema_prev;
}

auto TradingMethods::sma_m(const std::vector<double> &prices, int period) -> std::vector<double> {
    if (prices.size() < period) {
        return {};
    }

    std::vector<double> sma_values;
    double sum = std::accumulate(prices.begin(), prices.begin() + period, 0.0);
    sma_values.push_back(sum / period);

    for (size_t i = period; i < prices.size(); ++i) {
        sum += prices[i] - prices[i - period];
        sma_values.push_back(sum / period);
    }

    return sma_values;
}


auto TradingMethods::bollinger_bands(const std::vector<double> &prices, int period, double num_std_dev)
    -> std::tuple<double, double, double> {
    if (prices.size() < period) {
        return {0.0, 0.0, 0.0};
    }

    const double sum = std::accumulate(prices.end() - period, prices.end(), 0.0);
    const double mean = sum / period;

    double variance = 0.0;
    for (auto it = prices.end() - period; it != prices.end(); ++it) {
        variance += std::pow(*it - mean, 2);
    }
    variance /= (period - 1);
    const double stdev = std::sqrt(variance);

    const double upperBand = mean + (num_std_dev * stdev);
    const double lowerBand = mean - (num_std_dev * stdev);

    return std::make_tuple(lowerBand, mean, upperBand);
}
