#pragma once

#include "../../Common/include/BinanceScalping.h"

class OKXAPI : public BinanceScalping {
    OrderBookEntry order;

public:
    OKXAPI(const std::string &symbol, const int8_t &version, const std::string &host_,
           const std::string &port_, const std::string &target_)
        : order(OrderBookEntry(symbol, Market::OKX)),
          BinanceScalping(version, host_, port_, target_ + "?instId=" + symbol) {
    };

    void fetch_raw_data(std::vector<OrderBookEntry> &r_order) final;


};
