#include "../include/bayes_filtering_backtesting.h"
#include "../include/scalping_backtesting.h"
#include "../include/mean_reverse_backtesting.h"
#include "../include/arima_garch_adaptive_backtesting.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
