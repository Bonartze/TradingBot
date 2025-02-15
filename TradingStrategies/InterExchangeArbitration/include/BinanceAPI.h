#pragma once
#include "../../Common/include/BinanceScalping.h"

class BinanceAPI : public BinanceScalping {
private:
    OrderBookEntry order;

public:
    //! Getting access to API to extract price of the particular cryptocurrency
    BinanceAPI(const std::string &symbol, const int8_t &version, const std::string &host_,
               const std::string &port_,
               const std::string &target_) : order(OrderBookEntry(symbol, Market::BINANCE)),
                                             BinanceScalping(version, host_, port_,
                                                             target_ + "?symbol=" + symbol) {
    };

    void fetch_raw_data(std::vector<OrderBookEntry> &r_order) final;
};
