#pragma once
#include <memory>
#include "BinanceAPI.h"
#include "KrakenAPI.h"
#include "OKXAPI.h"
#include <thread>

class ExchangeAPI {
    static std::vector<std::unique_ptr<BinanceScalping> > apis;
    static std::vector<std::thread> api_threads;
    static std::vector<OrderBookEntry> orders;

public:
    static void run_interexchange_arbitrage(std::string crypto_pair);

};
