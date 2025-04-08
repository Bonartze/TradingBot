#pragma once
#include "../../Common/include/BinanceScalping.h"

class BinanceAPI : public BinanceScalping {
    OrderBookEntry order;

public:
    //! Getting access to API to extract price of the particular cryptocurrency
    BinanceAPI(const std::string &symbol, const int8_t &version, const std::string &host_,
               const std::string &port_,
               const std::string &target_) : BinanceScalping(version, host_, port_, target_ + "?symbol=" + symbol),
                                             order(OrderBookEntry(symbol, Market::BINANCE)) {
    };

    auto fetch_raw_data(std::vector<OrderBookEntry> &r_order) -> void final;
};
