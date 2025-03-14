#include "../include/TradingStrategy.h"
#include <algorithm>

auto TradingStrategy::extract_prices(const std::vector<Candle> &candles) -> std::vector<double> {
    std::vector<double> prices(candles.size());
    std::transform(candles.begin(), candles.end(), prices.begin(),
                   [](const Candle &candle) { return candle.close; });
    Logger(LogLevel::DEBUG) << "Extracted prices: " << prices.size();
    return prices;
}

TradingStrategy::TradingStrategy(const TradingParams &trading_params_, double balance_, bool position_open_,
                                 double entry_price_, double asset_quantity_, const std::string &symbol_,
                                 const std::string &key, const std::string &secret, bool is_backtesting_)
    : trading_params(trading_params_), balance(balance_), position_open(position_open_),
      entry_price(entry_price_), asset_quantity(asset_quantity_), symbol(symbol_), is_backtesting(is_backtesting_) {
    binanceOrderManager = std::make_unique<BinanceOrderManager>(key, secret);
}
