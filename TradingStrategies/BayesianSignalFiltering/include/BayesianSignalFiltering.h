#pragma once

#include <vector>
#include "../../../Logger/include/CSVLogger.h"
#include "../../../Logger/include/Logger.h"
#include "../../Common/include/TradingStrategy.h"

class BayesianSignalFiltering : public TradingStrategy {

    double buyThreshold = 0.55;
    double sellThreshold = 0.55;
    double rsiThreshold = 60.0;

    auto should_buy(const std::vector<double> &prices, CSVLogger &csv_logger) -> bool override;

    auto should_sell(const std::vector<double> &prices, double entry_price, CSVLogger &csv_logger) -> bool override;

    auto historical_up_probability(const std::vector<double> &prices, CSVLogger &csv_logger) -> double;

    auto historical_down_probability(const std::vector<double> &prices, CSVLogger &csv_logger) -> double;


    auto signal_probability_given_up(const std::vector<double> &prices,
                                     const std::vector<double> &rsi_values,
                                     double lower_band) -> double;

    auto signal_probability_given_down(const std::vector<double> &prices, const std::vector<double> &rsi_values,
                                       double upper_band) -> double;

    auto signal_frequency(const std::vector<double> &prices,
                          const std::vector<double> &rsi_values,
                          double lower_band) -> double;

public:
    BayesianSignalFiltering(): TradingStrategy({}, 1000, false, 0.0, 0.0, "key", "secret", "BTCUSDT") {
    }

    BayesianSignalFiltering(const TradingParams &trading_params, double balance_, bool is_position_open,
                            double quantity,
                            double entry_price, const std::string &key, const std::string &secret,
                            const std::string &symbol_, bool is_backtesting = true): TradingStrategy(
        trading_params, balance_, is_position_open, entry_price, quantity, key, secret, symbol_, is_backtesting ) {
    }

    auto set_parameters(const std::vector<double> &) -> void;


    auto execute(const std::vector<double> &prices, CSVLogger &csv_logger) -> double override;

    auto wrapper_execute(size_t window_size, const std::vector<double> &prices,
                         CSVLogger &csv_logger)
        -> std::pair<double, double>;
};
