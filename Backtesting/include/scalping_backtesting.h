#pragma once
#include <gtest/gtest.h>
#include <filesystem>
#include "../TradingStrategies/Scalping/ScalpingStr.h"

class Backtesting : public ::testing::Test {
public:
    std::string data_dir;
    ScalpingStr scalp;
protected:
    void SetUp() override;

    void TearDown() override;
};
