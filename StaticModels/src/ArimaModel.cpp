#include "../include/ArimaModel.h"

#include <numeric>

auto ARIMAModel::compute_first_diff(const std::vector<double> &series) -> std::vector<double> {
    std::vector<double> first_diff;
    for (size_t i = 1; i < series.size(); ++i)
        first_diff.emplace_back(series[i] - series[i - 1]);
    return first_diff;
}


auto ARIMAModel::Dikey_Fuller_test(const std::vector<double> &series) -> bool {
    size_t n = series.size() - 1;

    auto first_diff = compute_first_diff(series);

    Eigen::MatrixXf X(n, 3);
    Eigen::VectorXf Y(n);
    for (size_t i = 0; i < n; ++i) {
        X(i, 0) = 1;
        X(i, 1) = i + 1;
        X(i, 2) = series[i];
        Y[i] = first_diff[i];
    }

    Eigen::VectorXf beta_coeffs = (X.transpose() * X).inverse() * X.transpose() * Y;

    Eigen::VectorXf epsilon(n);
    for (size_t t = 0; t < n; ++t) {
        epsilon[t] = Y[t] - (beta_coeffs[0] + beta_coeffs[1] * (t + 1) + beta_coeffs[2] * series[t]);
    }

    double sigma2 = 0.0;
    for (const auto &eps: epsilon) {
        sigma2 += eps * eps;
    }
    sigma2 /= (n - 3);

    Eigen::MatrixXf cov_beta = sigma2 * (X.transpose() * X).inverse();

    double SE_gamma = std::sqrt(cov_beta(2, 2));

    double tau = beta_coeffs[2] / SE_gamma;

    double critical_value_5 = -3.41; // for 5% significance level

    if (tau < critical_value_5)
        return true;
    return false;
}

auto ARIMAModel::arima_parameters_evaluation(const std::vector<double> &series) -> std::tuple<double, double, double> {
    // d evaluation
    int d = 0;
    std::vector<double> diff_series = series;

    while (!Dikey_Fuller_test(diff_series)) {
        diff_series = compute_first_diff(diff_series);
        d++;
    }
}
