#pragma once
#include <gtest/gtest.h>
#include <filesystem>
#include <memory>
#include "../../TradingStrategies/ArimaGarchAdaptiveTrading/include/ArimaGarchAdaptive.h"

class ArimaGarchAdaptiveBacktesting : public ::testing::Test {
public:
    std::string test_file;
    std::unique_ptr<ArimaGarchAdaptive> arima_garch_adaptive_backtesting;

protected:
    void SetUp() override;
    void TearDown() override;
};
