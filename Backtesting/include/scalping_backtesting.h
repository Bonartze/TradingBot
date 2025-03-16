#pragma once
#include <gtest/gtest.h>
#include <filesystem>
#include "../../TradingStrategies/Scalping/include/ScalpingStr.h"

class ScalpingBacktesting : public ::testing::Test {
public:
    std::string data_dir;
    ScalpingStr scalp;

protected:
    void SetUp() override {
    };

    void TearDown() override {
    };
};
