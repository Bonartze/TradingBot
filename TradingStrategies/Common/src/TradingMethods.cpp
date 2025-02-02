#include "../include/TradingMethods.h"
#include <numeric>
#include <iostream>

auto TradingMethods::sma(const std::vector<double> &prices, size_t period) -> double {
    if (prices.size() < period) {
        throw std::invalid_argument("Not enough prices provided");
    }
    const double sum = std::accumulate(prices.end() - static_cast<std::ptrdiff_t>(period), prices.end(), 0.0);
    return sum / static_cast<double>(period);
}

auto TradingMethods::rsi(const std::vector<double> &prices, int period) -> double {
    if (prices.size() < period) {
        throw std::invalid_argument("Not enough prices provided");
    }

    double gain = 0.0;
    double loss = 0.0;
    for (size_t i = 1; i < period; ++i) {
        const double change = prices[i] - prices[i - 1];
        if (change > 0) {
            gain += change;
        } else {
            loss -= change;
        }
    }

    gain /= period;
    loss /= period;

    if (loss == 0) {
        return 100.0;
    }

    const double rs_ = gain / loss;
    return 100.0 - (100.0 / (1.0 + rs_));
}

auto TradingMethods::ema(const std::vector<double> &prices, size_t window_size) -> double {
    if (prices.size() < window_size) {
        throw std::invalid_argument("Not enough data to calculate EMA.");
    }

    double k = 2.0 / (window_size + 1);

    double ema_prev = prices.back();

    for (size_t i = prices.size() - 2; i != static_cast<size_t>(-1); --i) {
        ema_prev = (prices[i] * k) + (ema_prev * (1 - k));
    }

    return ema_prev;
}