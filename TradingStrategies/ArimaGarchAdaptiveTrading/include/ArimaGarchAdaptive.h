#pragma once
#include <memory>
#include <vector>
#include "../../../StaticModels/include/GarchModel.h"
#include "../../../StaticModels/include/ArimaModel.h"
#include "../../../Logger/include/CSVLogger.h"
#include "../../../Logger/include/Logger.h"
#include "../../Common/include/TradingStrategy.h"


class ArimaGarchAdaptive : public TradingStrategy {
private:
    std::unique_ptr<GarchModel> garch_model;
    std::unique_ptr<ARIMAModel> arima_model;

    auto should_buy(const std::vector<double> &, CSVLogger &) -> bool override;

    auto should_sell(const std::vector<double> &, double entry_price, CSVLogger &) -> bool override;

public:
    ArimaGarchAdaptive(const std::string &filepath): TradingStrategy({}, 1000, false, 0.0, 0.0),
                                                     arima_model(std::make_unique<ARIMAModel>(filepath)) {
        garch_model = std::make_unique<GarchModel>(*arima_model);
    }

    auto execute(const std::vector<double> &prices, CSVLogger &csv_logger) -> double override;

    auto wrapper_execute(size_t window_size, const std::vector<double> &prices,
                         CSVLogger &csv_logger)
        -> std::pair<double, double>;
};
