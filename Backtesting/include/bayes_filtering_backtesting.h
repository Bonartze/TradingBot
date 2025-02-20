#pragma once
#include <gtest/gtest.h>
#include <filesystem>
#include "../../TradingStrategies/BayesianSignalFiltering/include/BayesianSignalFiltering.h"

class Backtesting : public ::testing::Test {
public:
    std::string data_dir;
    BayesianSignalFiltering bayes_filter;
protected:
    void SetUp() override;

    void TearDown() override;
};
