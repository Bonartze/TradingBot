#include "../include/ArimaModel.h"
#include <numeric>
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <Eigen/Dense>

ARIMAModel::ARIMAModel() {
    if (close_prices.empty()) {
        throw std::runtime_error("Error: close_prices is empty.");
    }
    params = arima_parameters_evaluation(close_prices);
    auto diff_close_prices = compute_first_diff(close_prices);
    if(diff_close_prices.empty()) {
        throw std::runtime_error("Error: Not enough data to compute first difference.");
    }
    coefficients = estimate_coefficients(diff_close_prices, params.p, params.q);
}

auto ARIMAModel::PACF(int k) -> double {
    if (k < 0) {
        throw std::runtime_error("Error: k must be non-negative.");
    }
    if (k == 0) {
        return 1.0;
    }
    if (close_prices.size() <= static_cast<size_t>(k)) {
        throw std::runtime_error("Error: k is larger than data size.");
    }

    std::vector<double> veck(close_prices.begin(), close_prices.begin() + k);

    Eigen::MatrixXd matk = Eigen::MatrixXd::Zero(k, k);
    for (int row = 0; row < k; row++) {
        for (int i = 0; i < k - row; i++)
            matk(row, row + i) = veck[i];
        for (int i = 0; i < row; i++)
            matk(row, row - i - 1) = veck[i + 1];
    }

    Eigen::VectorXd veck2(k);
    if (close_prices.size() < static_cast<size_t>(k + 1)) {
        throw std::runtime_error("Error: not enough data for PACF computation.");
    }
    for (int i = 0; i < k; i++) {
        veck2(i) = close_prices[i + 1];
    }

    Eigen::VectorXd sol = matk.fullPivLu().solve(veck2);
    return sol(k - 1);
}

auto ARIMAModel::PACF() -> std::vector<double> {
    if (close_prices.empty()) {
        throw std::runtime_error("Error: Time series is empty.");
    }
    size_t max_lags = std::min(close_prices.size(), static_cast<size_t>(50));
    std::vector<double> pacf(max_lags);
    for (size_t i = 0; i < max_lags; i++)
        pacf[i] = PACF(i);
    return pacf;
}

auto ARIMAModel::ACF() -> std::vector<double> {
    size_t n = close_prices.size();
    if (n == 0) {
        throw std::runtime_error("Error: Time series is empty.");
    }
    double mean = std::accumulate(close_prices.begin(), close_prices.end(), 0.0) / n;
    double variance = std::accumulate(close_prices.begin(), close_prices.end(), 0.0,
                                      [&](double acc, double price) {
                                          return acc + (price - mean) * (price - mean);
                                      }) / n;
    if (variance == 0) {
        throw std::runtime_error("Error: Variance is zero, ACF cannot be computed.");
    }
    std::vector<double> acf(n);
    for (size_t lag = 0; lag < n; ++lag) {
        double covariance = std::inner_product(close_prices.begin() + lag, close_prices.end(),
                                               close_prices.begin(), 0.0,
                                               std::plus<>(),
                                               [&](double x, double y) {
                                                   return (x - mean) * (y - mean);
                                               }) / n;
        acf[lag] = covariance / variance;
    }
    return acf;
}

auto ARIMAModel::compute_first_diff(const std::vector<double> &series) -> std::vector<double> {
    if (series.size() < 2) {
        throw std::runtime_error("Error: Series size must be at least 2 for differencing.");
    }
    std::vector<double> first_diff(series.size() - 1);
    std::transform(series.begin() + 1, series.end(), series.begin(), first_diff.begin(),
                   std::minus<>());
    return first_diff;
}

auto ARIMAModel::Dickey_Fuller_test(const std::vector<double> &series) -> bool {
    if (series.size() < 4) {
        throw std::runtime_error("Error: Series too short for Dickey-Fuller test.");
    }
    size_t n = series.size() - 1;
    auto first_diff = compute_first_diff(series);
    Eigen::MatrixXd X(n, 3);
    Eigen::VectorXd Y(n);
    for (size_t i = 0; i < n; ++i) {
        X(i, 0) = 1;
        X(i, 1) = i + 1;
        X(i, 2) = series[i];
        Y[i] = first_diff[i];
    }
    Eigen::VectorXd beta_coeffs = (X.transpose() * X).fullPivLu().solve(X.transpose() * Y);
    Eigen::VectorXd epsilon = Y - (X * beta_coeffs);
    double sigma2 = epsilon.squaredNorm() / (n - 3);
    Eigen::MatrixXd cov_beta = sigma2 * (X.transpose() * X).inverse();
    double SE_gamma = std::sqrt(cov_beta(2, 2));
    double tau = beta_coeffs(2) / SE_gamma;
    double critical_value_5 = -3.41; // for 5% significance level
    return tau < critical_value_5;
}

auto ARIMAModel::arima_parameters_evaluation(const std::vector<double> &series) -> ArimaParams {
    if(series.empty()) {
        throw std::runtime_error("Error: Series is empty in arima_parameters_evaluation.");
    }
    int d = 0;
    std::vector<double> diff_series = series;
    while (!Dickey_Fuller_test(diff_series)) {
        diff_series = compute_first_diff(diff_series);
        d++;
        if(diff_series.empty()) {
            throw std::runtime_error("Error: Differenced series became empty.");
        }
    }
    std::vector<double> pacf_values = PACF();
    double critical_value_pacf = 1.96 / std::sqrt(series.size());
    int p = 0;
    for (size_t i = 0; i < pacf_values.size(); ++i) {
        if (std::abs(pacf_values[i]) > critical_value_pacf) {
            p = std::max(p, static_cast<int>(i + 1));
        } else {
            break;
        }
    }
    std::vector<double> acf_values = ACF();
    double critical_value_acf = 1.96 / std::sqrt(series.size());
    int q = 0;
    for (size_t i = 0; i < acf_values.size(); ++i) {
        if (std::abs(acf_values[i]) > critical_value_acf) {
            q = std::max(q, static_cast<int>(i + 1));
        } else {
            break;
        }
    }
    return {d, p, q};
}

auto ARIMAModel::build_regression_matrix(const std::vector<double> &diff_series, int p, int q) -> Eigen::MatrixXd {
    if (diff_series.size() <= static_cast<size_t>(std::max(p, q))) {
        throw std::runtime_error("Error: Not enough data in diff_series to build regression matrix.");
    }
    size_t n = diff_series.size() - std::max(p, q);
    Eigen::MatrixXd X = Eigen::MatrixXd::Zero(n, p + q);
    for (size_t i = 0; i < n; ++i) {
        for (int j = 0; j < p; ++j) {
            X(i, j) = diff_series[i + p - j - 1];
        }
        for (int j = 0; j < q; ++j) {
            X(i, p + j) = 0.0;
        }
    }
    return X;
}

auto ARIMAModel::build_target_vector(const std::vector<double> &diff_series, int p) -> Eigen::VectorXd {
    if (diff_series.size() <= static_cast<size_t>(p)) {
        throw std::runtime_error("Error: Not enough data in diff_series to build target vector.");
    }
    size_t n = diff_series.size() - p;
    Eigen::VectorXd Y(n);
    for (size_t i = 0; i < n; ++i) {
        Y(i) = diff_series[i + p];
    }
    return Y;
}

auto ARIMAModel::update_errors(Eigen::MatrixXd &X, const std::vector<double> &diff_series, int p, int q,
                               const std::vector<double> &phi) -> void {
    size_t n = X.rows();
    size_t p_size = phi.size();
    for (size_t i = 0; i < n; ++i) {
        double predicted = 0.0;
        for (size_t j = 0; j < p_size; ++j) {
            predicted += phi[j] * diff_series[i + p - j - 1];
        }
        double error = diff_series[i + p] - predicted;
        for (size_t j = 0; j < X.cols() - p; ++j) {
            X(i, p + j) = error;
        }
    }
}

ARIMACoefficients ARIMAModel::estimate_coefficients(const std::vector<double> &diff_series, int p, int q) {
    if(diff_series.empty()) {
        throw std::runtime_error("Error: diff_series is empty in estimate_coefficients.");
    }
    Eigen::MatrixXd X = build_regression_matrix(diff_series, p, q);
    Eigen::VectorXd Y = build_target_vector(diff_series, p);

    Eigen::VectorXd beta = (X.transpose() * X).inverse() * X.transpose() * Y;
    std::vector<double> phi(beta.data(), beta.data() + p);
    std::vector<double> theta(beta.data() + p, beta.data() + p + q);

    update_errors(X, diff_series, p, q, phi);
    beta = (X.transpose() * X).inverse() * X.transpose() * Y;
    theta.assign(beta.data() + p, beta.data() + p + q);

    return {phi, theta};
}

auto ARIMAModel::forecast(int steps) -> std::vector<double> {
    if (close_prices.size() < static_cast<size_t>(params.p)) {
        throw std::runtime_error("Error: Not enough close_prices for forecast (p > data size).");
    }
    std::vector<double> forecasts(steps);
    std::vector<double> errors(params.q, 0.0);

    std::vector<double> last_p_values(close_prices.end() - params.p, close_prices.end());

    for (int i = 0; i < steps; ++i) {
        double forecast_value = 0.0;
        for (int j = 0; j < params.p; ++j) {
            forecast_value += coefficients.phi[j] * last_p_values[params.p - j - 1];
        }
        for (int j = 0; j < params.q; ++j) {
            forecast_value += coefficients.theta[j] * errors[params.q - j - 1];
        }
        forecasts[i] = forecast_value;

        last_p_values.erase(last_p_values.begin());
        last_p_values.push_back(forecast_value);

        errors.erase(errors.begin());
        errors.push_back(0.0);
    }

    if (params.d > 0) {
        std::vector<double> integrated_forecasts(steps);
        double last_value = close_prices.back();
        for (int i = 0; i < steps; ++i) {
            last_value += forecasts[i];
            integrated_forecasts[i] = last_value;
        }
        return integrated_forecasts;
    }
    return forecasts;
}
