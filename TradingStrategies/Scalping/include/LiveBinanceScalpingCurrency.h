#pragma once
#include "../../Common/include/BinanceScalping.h"

class LiveBinanceScalpingCurrency : public BinanceScalping {
private:
    std::vector<double> prices;

public:
    [[nodiscard]] auto get_prices() const -> const std::vector<double> &{
        return prices;
    }

    LiveBinanceScalpingCurrency(int8_t version, const std::string &host_, const std::string &port_,
                                const std::string &target_);

    void fetch_raw_data(size_t scalping_data_point);

    void fetch_raw_data(std::vector<OrderBookEntry> &v) override {}
};
