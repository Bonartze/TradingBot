#include "../include/TradingStrategy.h"
#include <algorithm>

auto TradingStrategy::should_buy(const std::vector<double> &, const TradingParams &, CSVLogger &) -> bool {
}

auto TradingStrategy::extract_prices(const std::vector<Candle> &candles) -> std::vector<double> {
    std::vector<double> prices(candles.size());
    std::transform(candles.begin(), candles.end(), prices.begin(),
                   [](const Candle &candle) { return candle.close; });
    Logger(LogLevel::DEBUG) << "Extracted prices: " << prices.size();
    return prices;
}

auto TradingStrategy::should_sell(const std::vector<double> &, const TradingParams &, double, CSVLogger &) -> bool {
}

auto TradingStrategy::execute(const std::vector<double> &, TradingParams, CSVLogger &) -> double {
}
