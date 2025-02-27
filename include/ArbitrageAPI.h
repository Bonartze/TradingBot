#pragma once

#include "StrategiesAPIExport.h"

#include <vector>
#include <string>
#include <memory>
#include "../TradingStrategies/IntraExchangeArbitration/include/LiveBinanceScalping.h"
#include "../TradingEngine/include/BinanceOrderManager.h"
#include "../Logger/include/CSVLogger.h"

class STRATEGIES_API Arbitrage {
private:
    std::unique_ptr<BinanceOrderManager> order_manager;
    std::unique_ptr<LiveBinanceScalping> binance_scalping;
    void do_order_sequence(const std::vector<std::string> &, double);

public:
  	Arbitrage();
    Arbitrage(const std::unordered_set<std::string> &symbols, int version, const std::string &apiKey, const std::string &secretKey);
    void find_arbitrage_opportunities(const std::string &source, double amount);
};
