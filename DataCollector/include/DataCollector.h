#pragma once
#include <string>
#include <vector>
#include "../../TradingStrategies/Common/include/Common.h"

class DataCollector {
public:
    DataCollector(const std::string& symbol, const std::string& interval);

    std::vector<Candle> collectData(size_t limit = 1000);

    std::vector<double> get_close_prices();

private:
    std::string symbol_;
    std::string interval_;

    std::string httpGet(const std::string& url);
};
