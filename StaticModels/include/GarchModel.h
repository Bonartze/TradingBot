#pragma once
#include <memory>
#include "../include/ArimaModel.h"

struct GarchParams {
    double omega;
    double alpha;
    double beta;
};

class GarchModel {
private:
    GarchParams params;
    std::unique_ptr<ARIMAModel> arima_model;

    auto log_likelihood(const std::vector<double> &) -> double;

    auto last_sigma2(const std::vector<double> &) -> double;

public:
    auto forecast(const std::vector<double> &, int) -> std::vector<double>;

    auto combined_forecast(const std::vector<double> &, int) -> std::vector<double>;
};
