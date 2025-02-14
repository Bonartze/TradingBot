#pragma once
#include "../../Common/include/BinanceScalping.h"

class BinanceAPI : BinanceScalping {
private:
    std::string currency_name;
    double local_price;

public:
    //! Getting access to API to extract price of the particular cryptocurrency
    BinanceAPI(const std::string &currency_name_, const int8_t &version, const std::string &host_,
               const std::string &port_,
               const std::string &target_) : currency_name(currency_name_),
                                             BinanceScalping(version, host_, port_,
                                                             target_ + "?symbol=" + currency_name_), local_price(0) {
    };

    void fetch_raw_data(size_t scalping_data_point = 1) override;
};
