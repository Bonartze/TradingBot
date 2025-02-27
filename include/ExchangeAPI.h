#pragma once

#include <vector>
#include <memory>
#include "StrategiesAPIExport.h"
#include "../TradingStrategies/InterExchangeArbitration/include/BinanceAPI.h"
#include "../TradingStrategies/InterExchangeArbitration/include/KrakenAPI.h"
#include "../TradingStrategies/InterExchangeArbitration/include/OKXAPI.h"
#include "../Logger/include/Logger.h"

class STRATEGIES_API ExchangeAPI {
public:
    static void run_interexchange_arbitrage(const std::string &crypto_pair);
};
