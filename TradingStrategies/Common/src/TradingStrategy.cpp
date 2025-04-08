#include "../include/TradingStrategy.h"
#include <algorithm>
#include "../../../Logger/include/Logger.h"

auto TradingStrategy::extract_prices(const std::vector<Candle> &candles) -> std::vector<double> {
    std::vector<double> prices(candles.size());
    std::transform(candles.begin(), candles.end(), prices.begin(),
                   [](const Candle &candle) { return candle.close; });
    Logger(LogLevel::DEBUG) << "Extracted prices: " << prices.size();
    return prices;
}


TradingStrategy::TradingStrategy(const TradingParams &trading_params_,
                                 const TradeValues &trade_values,
                                 const std::string &symbol_, //NOLINT
                                 const std::string &key,
                                 const std::string &secret)
    : is_backtesting(trade_values.is_backtesting),
      symbol(symbol_),
      trading_params(trading_params_),
      balance(trade_values.balance),
      position_open(trade_values.position_open),
      entry_price(trade_values.entry_price),
      asset_quantity(trade_values.asset_quantity) {
    binanceOrderManager = std::make_unique<BinanceOrderManager>(key, secret);
}
