#include "../include/GarchModel.h"
#include "../Common/include/MathStatistics.h"
#include <numeric>
#include <cmath>
#include <stdexcept>

auto GarchModel::log_likelihood(const std::vector<double> &residuals) -> double {
    if (residuals.empty()) {
        throw std::runtime_error("Error: residuals vector is empty.");
    }
    int length = static_cast<int>(residuals.size());
    auto sigma2_prev = MathStatistics::variance(residuals);
    double sigma2_t = 0.0;
    double log_likelihood = 0.0;
    for (size_t t = 0; t < static_cast<size_t>(length); t++) {
        sigma2_t = params.omega
                   + params.alpha * (t > 0 ? residuals[t - 1] * residuals[t - 1] : sigma2_prev)
                   + params.beta * sigma2_prev;
        if (sigma2_t <= 0) {
            throw std::runtime_error("Error: computed sigma2_t is non-positive.");
        }
        log_likelihood += -0.5 * (std::log(2 * M_PI) + std::log(sigma2_t) + (residuals[t] * residuals[t]) / sigma2_t);
        sigma2_prev = sigma2_t;
    }
    return log_likelihood;
}

auto GarchModel::last_sigma2(const std::vector<double> &residuals) -> double {
    if (residuals.empty()) {
        throw std::runtime_error("Error: residuals vector is empty.");
    }
    auto sigma2_prev = MathStatistics::variance(residuals);
    double sigma2_t = sigma2_prev;
    for (size_t t = 1; t < residuals.size(); t++) {
        sigma2_t = params.omega
                   + params.alpha * (residuals[t - 1] * residuals[t - 1])
                   + params.beta * sigma2_prev;
        sigma2_prev = sigma2_t;
    }
    return sigma2_t;
}

auto GarchModel::forecast(const std::vector<double> &residuals, int steps) -> std::vector<double> {
    if (residuals.empty()) {
        throw std::runtime_error("Error: residuals vector is empty.");
    }
    auto sigma2_prev = last_sigma2(residuals);
    std::vector<double> garch_predicted;
    double sigma2_forecast = 0.0;
    double last_residual = residuals.back();
    for (int s = 0; s < steps; s++) {
        double error = (s == 0 ? last_residual * last_residual : 0.0);
        sigma2_forecast = params.omega + params.alpha * error + params.beta * sigma2_prev;
        if (sigma2_forecast <= 0) {
            throw std::runtime_error("Error: forecasted sigma2 is non-positive.");
        }
        garch_predicted.emplace_back(sigma2_forecast);
        sigma2_prev = sigma2_forecast;
    }
    return garch_predicted;
}

auto GarchModel::combined_forecast(const std::vector<double> & /*unusedResiduals*/, int steps) -> std::vector<double> {
    auto arima_predicted = arima_model->forecast(steps);
    auto arimaResiduals = arima_model->get_residuals();
    auto garch_predicted = forecast(arimaResiduals, steps);

    std::vector<double> updated_forecast;
    for (size_t s = 0; s < static_cast<size_t>(steps); s++) {
        double mu = arima_predicted[s];
        double sigma = std::sqrt(garch_predicted[s]);
        double random_shock = MathStatistics::sample_standart_normal();
        double simulated_value = mu + sigma * random_shock;
        updated_forecast.emplace_back(simulated_value);
    }
    return updated_forecast;
}
