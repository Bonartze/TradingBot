#pragma once
#include "../../Common/include/BinanceScalping.h"

class KrakenAPI : public BinanceScalping {
    OrderBookEntry order;

public:
    KrakenAPI(const std::string &symbol, const int8_t &version, const std::string &host_,
              const std::string &port_,
              const std::string &target_) : order(OrderBookEntry(symbol, Market::KRAKEN)),
                                            BinanceScalping(version, host_, port_,
                                                            target_ + "?pair=" + symbol) {
    };

    void fetch_raw_data(std::vector<OrderBookEntry> &r_order) final;

};

