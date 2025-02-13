#pragma
#include <gtest/gtest.h>
#include <filesystem>
#include "../../TradingStrategies/MeanReversionStrategy/include/MeanReverseStr.h"

class Backtesting : public ::testing::Test {
public:
    std::string data_dir;
    MeanReverseStrategy mvs;

protected:
    void SetUp() override;

    void TearDown() override;
};
