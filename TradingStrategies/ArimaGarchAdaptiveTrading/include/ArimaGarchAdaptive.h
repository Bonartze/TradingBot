#pragma once

#include <memory>
#include <vector>
#include "../../../StaticModels/include/GarchModel.h"
#include "../../../StaticModels/include/ArimaModel.h"
#include "../../../Logger/include/CSVLogger.h"
#include "../../../Logger/include/Logger.h"
#include "../../Common/include/TradingStrategy.h"

constexpr int def_balance = 1000;

class ArimaGarchAdaptive : public TradingStrategy {
private:
    std::unique_ptr<GarchModel> garch_model;
    std::unique_ptr<ARIMAModel> arima_model;

    auto should_buy(const std::vector<double> &prices, CSVLogger &logger) -> bool override;

    auto should_sell(const std::vector<double> &prices, double entry_price, CSVLogger &logger) -> bool override;

public:
    explicit ArimaGarchAdaptive(const std::vector<double> &data) : TradingStrategy({}, def_balance, false, 0.0, 0.0, "",
                                                                                   "",
                                                                                   ""),
                                                                   arima_model(std::make_unique<ARIMAModel>(data)) {
        garch_model = std::make_unique<GarchModel>(*arima_model);
    }

    explicit ArimaGarchAdaptive(const std::string &data) : TradingStrategy({}, def_balance, false, 0.0, 0.0, "", "",
                                                                           ""),
                                                           arima_model(std::make_unique<ARIMAModel>(data)) {
        garch_model = std::make_unique<GarchModel>(*arima_model);
    }

    ArimaGarchAdaptive(const std::vector<double> &data, const TradingParams &trading_params, double balance,
                       bool is_position_open, double entry_price, double asset_quantity, const std::string &key,
                       const std::string &secret, const std::string &symbol, bool is_backtesting = true)
            : TradingStrategy(
            trading_params, balance,
            is_position_open, entry_price,
            asset_quantity, key, secret, symbol, is_backtesting),
              arima_model(
                      // another way -> at first gather data, then fill it out
                      std::make_unique<ARIMAModel>(
                              data)) {
        garch_model = std::make_unique<GarchModel>(*arima_model);
    }

    ArimaGarchAdaptive(const std::string &data, const TradingParams &trading_params, double balance,
                       bool is_position_open, double entry_price, double asset_quantity, const std::string &key,
                       const std::string &secret, const std::string &symbol, bool is_backtesting = true)
            : TradingStrategy(
            trading_params, balance,
            is_position_open, entry_price,
            asset_quantity, key, secret, symbol, is_backtesting),
              arima_model(
                      // another way -> at first gather data, then fill it out
                      std::make_unique<ARIMAModel>(
                              data)) {
        garch_model = std::make_unique<GarchModel>(*arima_model);
    }

    auto execute(const std::vector<double> &prices, CSVLogger &csv_logger) -> double override;

    auto wrapper_execute(size_t window_size, const std::vector<double> &prices,
                         CSVLogger &csv_logger)
    -> std::pair<double, double>;
};
