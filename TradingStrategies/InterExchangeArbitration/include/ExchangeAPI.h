#pragma once
#include <memory>
#include <vector>
#include <thread>
#include "../../Common/include/BinanceScalping.h"
#include "../../Common/include/Common.h"

class ExchangeAPI {
    static std::vector<std::unique_ptr<BinanceScalping> > apis;
    static std::vector<std::thread> api_threads;
    static std::vector<OrderBookEntry> orders;

public:
    static void run_interexchange_arbitrage(const std::string &crypto_pair);
};
