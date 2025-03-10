#include "../include/GarchModel.h"
#include <stdexcept>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>


static double sample_variance(const std::vector<double> &v) {
    if (v.size() < 2) return 0.0;
    double mean = 0.0;
    for (auto &x: v) mean += x;
    mean /= v.size();
    double var = 0.0;
    for (auto &x: v) {
        double d = x - mean;
        var += d * d;
    }
    var /= (v.size() - 1);
    return var;
}


static double sample_standard_normal() {
    static thread_local std::mt19937 gen(std::random_device{}());
    static thread_local std::normal_distribution<> dist(0.0, 1.0);
    return dist(gen);
}

GarchModel::GarchModel(const ARIMAModel &arima) {
    arima_model = std::make_unique<ARIMAModel>(arima);
}


void GarchModel::fit_garch_parameters(const std::vector<double> &residuals) {
    naive_grid_search(residuals);

    //std:: << "[GARCH::fit_garch_parameters] best (omega, alpha, beta) = ("
    //       << params.omega << ", " << params.alpha << ", " << params.beta << ")\n";
}


double GarchModel::log_likelihood(const std::vector<double> &residuals,
                                  double omega, double alpha, double beta) {
    if (residuals.empty()) return 0.0;

    double sigma2_prev = sample_variance(residuals);
    double ll = 0.0;

    for (size_t t = 0; t < residuals.size(); t++) {
        double sigma2_t = omega;
        if (t == 0) {
            sigma2_t += alpha * (residuals[0] * residuals[0]) + beta * sigma2_prev;
        } else {
            double e2 = residuals[t - 1] * residuals[t - 1];
            sigma2_t += alpha * e2 + beta * sigma2_prev;
        }
        if (sigma2_t <= 0.0) return -1e15;
        double r2 = residuals[t] * residuals[t];
        double term = -0.5 * (std::log(2.0 * M_PI) + std::log(sigma2_t) + r2 / sigma2_t);
        ll += term;
        sigma2_prev = sigma2_t;
    }
    return ll;
}


double GarchModel::last_sigma2(const std::vector<double> &residuals) {
    if (residuals.empty()) {
        throw std::runtime_error("Error: residuals are empty");
    }
    double sigma2_prev = sample_variance(residuals);
    for (size_t t = 1; t < residuals.size(); t++) {
        double e2 = residuals[t - 1] * residuals[t - 1];
        sigma2_prev = params.omega + params.alpha * e2 + params.beta * sigma2_prev;
    }
    return sigma2_prev;
}


std::vector<double> GarchModel::forecast(const std::vector<double> &residuals, int steps) {
    fit_garch_parameters(residuals);
    if (residuals.empty()) {
        throw std::runtime_error("Error: no residuals for GARCH forecast");
    }
    double sigma2_prev = last_sigma2(residuals);
    std::vector<double> forecasts;
    forecasts.reserve(steps);

    double last_res = residuals.back();
    for (int i = 0; i < steps; i++) {
        double e2 = (i == 0 ? last_res * last_res : 0.0);
        double max_volatility = 1e6;
        double sigma2_f = params.omega + params.alpha * e2 + params.beta * sigma2_prev;
        sigma2_f = std::min(sigma2_f, max_volatility);

        if (sigma2_f <= 0.0) {
            throw std::runtime_error("Forecasted sigma2 is non-positive.");
        }
        forecasts.push_back(sigma2_f);
        sigma2_prev = sigma2_f;
    }
    return forecasts;
}


std::vector<double> GarchModel::combined_forecast(int steps) {
    auto arima_pred = arima_model->forecast(steps);

    auto resid = arima_model->get_residuals();

    fit_garch_parameters(resid);
    auto garch_pred = forecast(resid, steps);

    std::vector<double> combined;
    combined.reserve(steps);

    for (int i = 0; i < steps; i++) {
        std::cout << "Step: " << i << std::endl;
        double mu = arima_pred[i];
        double sigma = std::sqrt(garch_pred[i]);
        double z = sample_standard_normal();


        double simulated_value = mu + 0.3 * sigma * z;


        combined.push_back(simulated_value);


        std::cout << "[combined_forecast] Step=" << i
                << ", ARIMA=" << mu
                << ", GARCH_var=" << garch_pred[i]
                << ", shock=" << z
                << ", => " << simulated_value << std::endl;
    }

    return combined;
}


void GarchModel::naive_grid_search(const std::vector<double> &residuals) {
    double best_omega = 0.1, best_alpha = 0.05, best_beta = 0.9;
    double best_ll = -std::numeric_limits<double>::infinity();


    const double step_w = 0.05;
    const double step_a = 0.05;
    const double step_b = 0.05;

    for (double w = 0.01; w < 1.0; w += step_w) {
        for (double a = 0.0; a < 1.0; a += step_a) {
            for (double b = 0.0; b < 1.0; b += step_b) {
                if (a + b >= 0.999) {
                    continue;
                }
                double ll = log_likelihood(residuals, w, a, b);
                if (ll > best_ll) {
                    best_ll = ll;
                    best_omega = w;
                    best_alpha = a;
                    best_beta = b;
                }
            }
        }
    }
    params = {best_omega, best_alpha, best_beta};
    //std::cout << "[GARCH::grid_search] best log-likelihood=" << best_ll << std::endl;
}
